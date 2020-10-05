#include "Boid.h"
#include "Kismet/KismetMathLibrary.h"

ABoid::ABoid()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ABoid::BeginPlay()
{
	Super::BeginPlay();

	float angle = FMath::FRandRange(0, 3.14*2);
	Velocity = FVector(FMath::FRandRange(-1,1), FMath::FRandRange(-1, 1), 0);
}

void ABoid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//auto position = GetActorLocation();
	//SetActorLocation(position + FVector(1, 0, 0));
}

void ABoid::Run(const TArray<ABoid*>& boids)
{
	Flock(boids);
	Update();
	Borders();
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

	sep *= 1.5f;
	ali *= 1.0f;
	coh *= 1.0f;

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
	SetActorLocation(position + Velocity);
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
	FVector position = GetActorLocation();

}

FVector ABoid::Separate(const TArray<ABoid*>& boids)
{
	FVector position = GetActorLocation();
	float desiredSeparation = 25.0f;
	FVector steer;

	for (ABoid* other : boids)
	{
		float d = FVector::Dist(position, other->GetActorLocation());
		if(d > 0 && d < desiredSeparation)
		{
			FVector diff = position - other->GetActorLocation();
			diff.Normalize();
			diff /= d;
			steer += diff;
		}
	}

	if(boids.Num() > 0)
	{
		steer /= boids.Num();
	}

	if(steer.Size() > 0)
	{
		steer.Normalize();
		steer *= MaxSpeed;
		steer /= Velocity;
		steer = UKismetMathLibrary::ClampVectorSize(steer, 0, MaxForce);
	}
	return steer;
}

FVector ABoid::Align(const TArray<ABoid*>& boids)
{
	float neighborDist = 50;
	FVector sum;
	FVector position = GetActorLocation();

	for(ABoid* other : boids)
	{
		float d = FVector::Dist(position, other->GetActorLocation());
		if(d > 0 && d < neighborDist)
		{
			sum += other->Velocity;
		}
	}

	FVector steer;
	if(boids.Num())
	{
		sum /= boids.Num();
		sum.Normalize();
		sum *= MaxSpeed;
		steer = sum - Velocity;
		steer = UKismetMathLibrary::ClampVectorSize(steer, 0, MaxForce);
	}
	return steer;
}

FVector ABoid::Cohesion(const TArray<ABoid*>& boids)
{
	float neighborDist = 50;
	FVector sum;
	FVector position = GetActorLocation();

	for(ABoid* other : boids)
	{
		float d = FVector::Dist(position, other->GetActorLocation());
		if(d > 0 && d < neighborDist)
		{
			sum += other->GetActorLocation();
		}
	}

	if(boids.Num())
	{
		sum /= boids.Num();
		return Seek(sum);
	}
	return FVector::ZeroVector;
}