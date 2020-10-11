#include "Boid.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

ABoid::ABoid()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ABoid::BeginPlay()
{
	Super::BeginPlay();

	Velocity = FVector(FMath::FRandRange(-1,1), FMath::FRandRange(-1, 1), 0);
	Velocity *= MaxSpeed;
}

void ABoid::Tick(float DeltaSeconds)
{

}

void ABoid::Run(const TArray<ABoid*>& boids)
{
	Flock(boids);
	Update();
	Borders();
}

void ABoid::Setup(float width, float height)
{
	Width = width;
	Height = height;
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

	sep *= SeparationMult;
	ali *= AlignMult;
	coh *= ApproachMult;

	if(DrawDebug)
	{
		UE_LOG(LogTemp, Log, TEXT("Separate: %s"), *sep.ToString());
		UE_LOG(LogTemp, Log, TEXT("Align: %s"), *ali.ToString());
		UE_LOG(LogTemp, Log, TEXT("Cohesion: %s"), *coh.ToString());
	}

	ApplyForce(sep);
	ApplyForce(ali);
	ApplyForce(coh);
}

void ABoid::Update()
{
	Acceleration.Z = 0;
	Velocity += Acceleration;
	Velocity = UKismetMathLibrary::ClampVectorSize(Velocity, 0, MaxSpeed);

	auto position = GetActorLocation();
	auto newPosition = position + Velocity;
	SetActorRotation(UKismetMathLibrary::MakeRotFromX(newPosition - position));
	SetActorLocation(newPosition);
	Acceleration *= 0;

	if (DrawDebug)
	{
		UE_LOG(LogTemp, Log, TEXT("Velocity: %f"), Velocity.Size());

		UKismetSystemLibrary::DrawDebugSphere(GetWorld(), GetActorLocation(),
			ApproachDistance, 12, FLinearColor::Blue);
	}
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
	float halfWidth = Width / 2;
	float halfHieght = Height / 2;

	FVector position = GetActorLocation();
	if (position.X < -halfWidth-R) position.X = halfWidth + R;
	if (position.Y < -halfHieght -R) position.Y = halfHieght + R;
	if (position.X > halfWidth + R) position.X = -halfWidth - R;
	if (position.Y > halfHieght + R) position.Y = -halfHieght - R;
	SetActorLocation(position);
}

FVector ABoid::Separate(const TArray<ABoid*>& boids)
{
	FVector position = GetActorLocation();
	FVector steer(0,0,0);

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
		steer /= count;
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
	FVector sum(0, 0, 0);
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

	FVector steer(0, 0, 0);
	if(count > 0)
	{
		sum /= count;
		sum.Normalize();
		sum *= MaxSpeed;
		steer = sum - Velocity;
		steer = UKismetMathLibrary::ClampVectorSize(steer, 0, MaxForce);
	}
	return steer;
}

FVector ABoid::Cohesion(const TArray<ABoid*>& boids)
{
	FVector sum(0, 0, 0);
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
		sum /= count;
		return Seek(sum);
	}
	return FVector::ZeroVector;
}