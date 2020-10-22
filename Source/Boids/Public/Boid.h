#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Boid.generated.h"

UCLASS()
class BOIDS_API ABoid : public AActor
{
	GENERATED_BODY()
	
public:	
	ABoid();

	void Run(const TArray<ABoid*>& boids);
	void Setup(FVector lowerBound, FVector upperBound, const TArray<FVector>& directions, const FLinearColor& color);

protected:

	UPROPERTY(EditDefaultsOnly)
	class UStaticMeshComponent* Mesh;
	UPROPERTY(EditDefaultsOnly)
	class USceneComponent* Root;

	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly)
	float MaxForce = 0.06f;
	UPROPERTY(EditDefaultsOnly)
	float MaxSpeed = 4.0f;

	UPROPERTY(EditDefaultsOnly)
	float SeparationMult = 5.0f;
	UPROPERTY(EditDefaultsOnly)
	float AlignMult = 3.0f;
	UPROPERTY(EditDefaultsOnly)
	float ApproachMult = 1.0f;
	UPROPERTY(EditDefaultsOnly)
	float ObstacleMult = 6.0f;

	UPROPERTY(EditDefaultsOnly)
	float SeparationDistance = 50.0f;
	UPROPERTY(EditDefaultsOnly)
	float AlignDistance = 100.0f;
	UPROPERTY(EditDefaultsOnly)
	float ApproachDistance = 100.0f;
	UPROPERTY(EditDefaultsOnly)
	float ObstacleDistance = 500.0f;

	UPROPERTY(EditDefaultsOnly)
	bool BordersWarpAround = false;

private:
	void ApplyForce(FVector force);
	void Flock(const TArray<ABoid*>& boids);
	void Update();

	FVector Seek(FVector target);
	void Borders();
	FVector Separate(const TArray<ABoid*>& boids);
	FVector Align(const TArray<ABoid*>& boids);
	FVector Cohesion(const TArray<ABoid*>& boids);
	FVector Obstacle();

	bool CheckDirection(const FVector& dir);

	FVector Velocity;
	FVector Acceleration;

	float R = 2.0f;
	
	FVector LowerBound;
	FVector UpperBound;

	TArray<FVector> Directions;

};