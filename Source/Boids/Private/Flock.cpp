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
		ABoid* boid = GetWorld()->SpawnActor<ABoid>(BoidBP, FVector(0, 0, 50), FRotator(), spawnParameters);
		boids.Add(boid);
	}
}

void AFlock::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	for (ABoid* boid : boids)
	{
		boid->Run(boids);
	}
}