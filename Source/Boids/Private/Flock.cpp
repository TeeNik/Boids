#include "Flock.h"
#include "Boid.h"

AFlock::AFlock()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AFlock::BeginPlay()
{
	Super::BeginPlay();

	CalculateDirections();

	FActorSpawnParameters spawnParameters;
	spawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	for(int i = 0; i < Num; ++i)
	{
		FVector pos = FVector(FMath::FRandRange(LowerBound.X, UpperBound.X),
			FMath::FRandRange(LowerBound.Y, UpperBound.Y),
			FMath::FRandRange(LowerBound.Z, UpperBound.Z));

		ABoid* boid = GetWorld()->SpawnActor<ABoid>(BoidBP, pos, FRotator(), spawnParameters);
		boid->Setup(LowerBound, UpperBound, Directions);
		Boids.Add(boid);
	}

	//if(Boids.Num())
	//{
	//	Boids[0]->DrawDebug = true;
	//}
}

void AFlock::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	for (ABoid* boid : Boids)
	{
		boid->Run(Boids);
	}
}

void AFlock::CalculateDirections()
{
	Directions.Reserve(NumOfDirections);
	float goldenRatio = (1 + FMath::Sqrt(5)) / 2;
	float angleIncrement = PI * 2 * goldenRatio;

	for (int i = 0; i < NumOfDirections; ++i) {
		float t = (float)i / NumOfDirections;
		float inclination = FMath::Acos(1 - 2 * t);
		float azimuth = angleIncrement * i;

		float x = FMath::Sin(inclination) * FMath::Cos(azimuth);
		float y = FMath::Sin(inclination) * FMath::Sin(azimuth);
		float z = FMath::Cos(inclination);
		Directions.Add(FVector(x, y, z));
	}
}