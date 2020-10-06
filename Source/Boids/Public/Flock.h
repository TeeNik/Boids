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
	float Width = 2000;
	UPROPERTY(EditDefaultsOnly)
	float Height = 1500;
	UPROPERTY(EditDefaultsOnly)
	int Num = 30;

private:
	TArray<ABoid*> boids;

};