#include "Boid.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"

ABoid::ABoid()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	RootComponent = Root;
	Mesh->SetupAttachment(Root);
}

void ABoid::BeginPlay()
{
	Super::BeginPlay();

	Velocity = FVector(FMath::FRandRange(-1,1), FMath::FRandRange(-1, 1), FMath::FRandRange(-1, 1));
	Velocity *= MaxSpeed;
}

void ABoid::Run(const TArray<ABoid*>& boids, float deltaTime)
{
	Flock(boids);
	Update(deltaTime);
	Borders();
}

void ABoid::Setup(FVector lowerBound, FVector upperBound, const TArray<FVector>& directions, const FLinearColor& color)
{
	LowerBound = lowerBound;
	UpperBound = upperBound;
	Directions = directions;

	UMaterialInstanceDynamic* MI = UMaterialInstanceDynamic::Create(Mesh->GetMaterial(0), this);
	Mesh->SetMaterial(0, MI);
	MI->SetVectorParameterValue(TEXT("Color"), color);
	MI->SetVectorParameterValue(TEXT("Emissive"), color);
}

void ABoid::ApplyForce(FVector force)
{
	Acceleration += force;
}

void ABoid::Flock(const TArray<ABoid*>& boids)
{
	FVector sep = Separate(boids);
	FVector ali = Align(boids);
	FVector coh = Cohesion(boids);
	FVector obs = Obstacle();

	sep *= SeparationMult;
	ali *= AlignMult;
	coh *= ApproachMult;
	obs *= ObstacleMult;

	ApplyForce(sep);
	ApplyForce(ali);
	ApplyForce(coh);
	ApplyForce(obs);
}

void ABoid::Update(float deltaTime)
{
	Velocity += Acceleration * deltaTime * 10;
	Velocity = UKismetMathLibrary::ClampVectorSize(Velocity, 0, MaxSpeed);

	auto position = GetActorLocation();
	auto newPosition = position + Velocity;

	FVector dir = newPosition - position;
	dir.Normalize();
	SetActorRotation(FMath::Lerp(GetActorRotation(), dir.Rotation(), 0.05f));
	
	SetActorLocation(newPosition);
	Acceleration *= 0;
}

FVector ABoid::Seek(FVector target)
{
	FVector position = GetActorLocation();
	FVector desired = target - position;
	desired.Normalize();
	desired *= MaxSpeed;

	FVector steer = desired - Velocity;
	steer = UKismetMathLibrary::ClampVectorSize(steer, 0, MaxForce);
	return steer;
}

void ABoid::Borders()
{
	if(BordersWarpAround)
	{
		FVector position = GetActorLocation();
		if (position.X < LowerBound.X - R) position.X = UpperBound.X - R;
		if (position.Y < LowerBound.Y - R) position.Y = UpperBound.Y - R;
		if (position.Z < LowerBound.Z - R) position.Z = UpperBound.Z - R;
		if (position.X > UpperBound.X + R) position.X = LowerBound.X + R;
		if (position.Y > UpperBound.Y + R) position.Y = LowerBound.Y - R;
		if (position.Z > UpperBound.Z + R) position.Z = LowerBound.Z - R;
		SetActorLocation(position);
	}
}

FVector ABoid::Separate(const TArray<ABoid*>& boids)
{
	FVector position = GetActorLocation();
	FVector steer(0);

	int count = 0;
	for (ABoid* other : boids)
	{
		float d = FVector::Dist(position, other->GetActorLocation());
		if(d > 0 && d < SeparationDistance)
		{
			FVector diff = position - other->GetActorLocation();
			diff.Normalize();
			diff /= d;
			steer += diff;
			++count;
		}
	}

	if(count > 0)
	{
		steer /= (float)count;
	}

	if(steer.Size() > 0)
	{
		steer.Normalize();
		steer *= MaxSpeed;
		steer -= Velocity;
		steer = UKismetMathLibrary::ClampVectorSize(steer, 0, MaxForce);
	}
	return steer;
}

FVector ABoid::Align(const TArray<ABoid*>& boids)
{
	FVector sum(0);
	FVector position = GetActorLocation();

	int count = 0;
	for(ABoid* other : boids)
	{
		float d = FVector::Dist(position, other->GetActorLocation());
		if(d > 0 && d < AlignDistance)
		{
			sum += other->Velocity;
			++count;
		}
	}

	FVector steer(0);
	if(count > 0)
	{
		sum /= (float)count;
		sum.Normalize();
		sum *= MaxSpeed;
		steer = sum - Velocity;
		steer = UKismetMathLibrary::ClampVectorSize(steer, 0, MaxForce);
	}
	return steer;
}

FVector ABoid::Cohesion(const TArray<ABoid*>& boids)
{
	FVector sum(0);
	FVector position = GetActorLocation();

	int count = 0;
	for(ABoid* other : boids)
	{
		float d = FVector::Dist(position, other->GetActorLocation());
		if(d > 0 && d < ApproachDistance)
		{
			sum += other->GetActorLocation();
			++count;
		}
	}

	if(count > 0)
	{
		sum /= (float)count;
		return Seek(sum);
	}
	return FVector::ZeroVector;
}

FVector ABoid::Obstacle()
{
	FVector resultDir(0);

	if(CheckDirection(GetActorForwardVector()))
	{
		for (int i = 0; i < Directions.Num(); ++i)
		{
			const FTransform transform = Mesh->GetComponentTransform();
			FVector dir = UKismetMathLibrary::TransformDirection(transform, Directions[i]);
			
			if(!CheckDirection(dir))
			{
				resultDir = dir;
				resultDir *= MaxSpeed;
				resultDir -= Velocity;
				resultDir = UKismetMathLibrary::ClampVectorSize(dir, 0, MaxForce);
				break;
			}
		}
	}

	return resultDir;
}

bool ABoid::CheckDirection(const FVector& dir)
{
	FVector start = GetActorLocation();
	FVector end = start + dir * ObstacleDistance;

	FCollisionQueryParams CollisionParams;
	FHitResult hit;

	return GetWorld()->LineTraceSingleByChannel(hit, start, end, ECC_WorldStatic, CollisionParams);
}