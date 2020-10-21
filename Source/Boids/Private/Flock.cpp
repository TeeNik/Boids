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

		FLinearColor color = BoidColors[FMath::RandRange(0, BoidColors.Num() - 1)];

		ABoid* boid = GetWorld()->SpawnActor<ABoid>(BoidBP, pos, FRotator(), spawnParameters);
		boid->Setup(LowerBound, UpperBound, Directions, color);
		//boid->DrawDebug = true;
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
	float angleStep = PI * 2 * goldenRatio;

	for (int i = 0; i < NumOfDirections; ++i) {
		float t = (float)i / NumOfDirections;
		float slope = FMath::Acos(1 - 2 * t);
		float azimuth = angleStep * i;

		float x = FMath::Sin(slope) * FMath::Cos(azimuth);
		float y = FMath::Sin(slope) * FMath::Sin(azimuth);
		float z = FMath::Cos(slope);
		Directions.Add(FVector(x, y, z));
	}
}