#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/ActorComponent.h"
#include "TargetSensingComponent.generated.h"

// handles any incoming enemies in the given vicinity
// 

class UBoxComponent;
class UCameraComponent;

const float SQUARED_DISTANCE_SENSING_RANGE = 800000.f;

UCLASS(ClassGroup = (Custom), meta = (Blue))
class NUJASCOMBAT_API AActorSensor : public AActor
{
	GENERATED_BODY()

public:
	AActorSensor();
protected:
	virtual void BeginPlay() override;
public:
	virtual void Tick(float DeltaSeconds) override;

};


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class NUJASCOMBAT_API UTargetSensingComponent : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY()
	TSharedPtr<UCameraComponent> PlayerCamera = nullptr;
	UPROPERTY()
	AActor* ClosestEnemy = nullptr;
	UPROPERTY()
	TMap<AActor*, AActorSensor*> SensorMap;


	bool IsActorWithinScreenRange(AActor* Enemy, float& DotProduct); // idk why have the dot product yet
	bool IsWithinSensingRange(float SquaredDistanceToEnemy);

public:
	// Sets default values for this component's properties
	UTargetSensingComponent();

	UPROPERTY(BlueprintReadWrite, Category="Target Sensing")
	float SquaredDistanceSensingRange = SQUARED_DISTANCE_SENSING_RANGE;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

	void AddEnemy(AActor* Enemy);
	void RemoveEnemy(AActor* Enemy);
	AActor* SearchForClosestEnemy();
	AActor* GetClosestEnemy();
	int32 EnemyCountInBounds();
};