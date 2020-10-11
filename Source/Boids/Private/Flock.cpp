#include "Flock.h"
#include "Boid.h"

AFlock::AFlock()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AFlock::BeginPlay()
{
	Super::BeginPlay();

	FActorSpawnParameters spawnParameters;
	spawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	for(int i = 0; i < Num; ++i)
	{
		auto pos = FVector(FMath::RandRange(-Width / 2, Width / 2),
			FMath::RandRange(-Height / 2, Height / 2), 50);

		ABoid* boid = GetWorld()->SpawnActor<ABoid>(BoidBP, pos, FRotator(), spawnParameters);
		boid->Setup(Width, Height);
		boids.Add(boid);
	}

	//if(boids.Num())
	//{
	//	boids[0]->DrawDebug = true;
	//}
}

void AFlock::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	for (ABoid* boid : boids)
	{
		boid->Run(boids);
	}
}