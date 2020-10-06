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
	void Setup(float width, float height);

	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly)
	float MaxForce = 0.05f;
	UPROPERTY(EditDefaultsOnly)
	float MaxSpeed = 5.0f;

private:
	void ApplyForce(FVector force);
	void Flock(const TArray<ABoid*>& boids);
	void Update();

	FVector Seek(FVector target);
	void Borders();
	FVector Separate(const TArray<ABoid*>& boids);
	FVector Align(const TArray<ABoid*>& boids);
	FVector Cohesion(const TArray<ABoid*>& boids);

	FVector Velocity;
	FVector Acceleration;

	float R = 2.0f;
	
	float Width;
	float Height;

};
