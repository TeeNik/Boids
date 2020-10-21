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
	UPROPERTY(EditDefaultsOnly)
	int Num = 30;

	UPROPERTY(EditDefaultsOnly)
	FVector LowerBound;
	UPROPERTY(EditDefaultsOnly)
	FVector UpperBound;

	UPROPERTY(EditDefaultsOnly)
	TArray<FLinearColor> BoidColors;

private:
	TArray<ABoid*> Boids;
	
	const int NumOfDirections = 300;
	TArray<FVector> Directions;

	void CalculateDirections();

};