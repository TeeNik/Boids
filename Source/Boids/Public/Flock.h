#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Flock.generated.h"

class ABoid;

UCLASS()
class BOIDS_API AFlock : public AActor
{
	GENERATED_BODY()
	
public:	
	AFlock();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ABoid> BoidBP;

private:
	TArray<ABoid*> boids;
	const int Num = 1;

	float width = 2000;
	float height = 1500;

};