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
const float SCREEN_RANGE = 0.15f;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FEnemyDetectionDelegate);


USTRUCT()
struct FSensingDistanceResponse
{
	GENERATED_BODY()

	bool bIsInRange;
	float DistSquaredFromSensorToTarget;
};

USTRUCT()
struct FSensingViewSpaceResponse
{
	GENERATED_BODY()

	bool bIsInView;
	float DotProductFromViewToTarget;
};

// This is a sensor that would point in the direction of enemies around.
// Implementation TBA
UCLASS(ClassGroup = (Custom), meta = (Blue))
class NUJASCOMBAT_API AActorSensor : public AActor
{
	GENERATED_BODY()

public:
	AActorSensor();
protected:
	virtual void BeginPlay() override;
};


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class NUJASCOMBAT_API UTargetSensingComponent : public UActorComponent
{
	GENERATED_BODY()

	TSharedPtr<UCameraComponent> PlayerCamera = nullptr; // cannot be assigned as a property
	UPROPERTY()
	AActor* Owner = nullptr;
	UPROPERTY()
	AActor* ClosestEnemy = nullptr;
	UPROPERTY()
	float DotProdToClosestEnemy = NAN;
	UPROPERTY()
	float DistSqrdToClosestEnemy = NAN;
	UPROPERTY()
	TMap<AActor*, AActorSensor*> SensorMap;

	inline FSensingViewSpaceResponse IsEnemyInScreenView(const AActor* Enemy);
	inline FSensingDistanceResponse IsEnemyInSensingDistance(const AActor* Enemy);

public:
	// Sets default values for this component's properties
	UTargetSensingComponent();

	UPROPERTY(BlueprintReadWrite, Category="Target Sensing")
	float SquaredDistanceSensingRange = SQUARED_DISTANCE_SENSING_RANGE;
	UPROPERTY(BlueprintReadWrite, Category="Target Sensing")
	float ScreenRange = SCREEN_RANGE;

	UPROPERTY(BlueprintReadWrite, Category="Target Sensing")
	FEnemyDetectionDelegate OnEnemyEnter;
	UPROPERTY(BlueprintReadWrite, Category="Target Sensing")
	FEnemyDetectionDelegate OnEnemyExit;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category="Target Sensing")
	void AddEnemy(AActor* Enemy);
	UFUNCTION(BlueprintCallable, Category="Target Sensing")
	void RemoveEnemy(AActor* Enemy);
	UFUNCTION(BlueprintCallable, Category="Target Sensing")
	AActor* SearchForClosestEnemy();
	UFUNCTION(BlueprintPure, Category="Target Sensing")
	inline AActor* GetClosestEnemy();
	UFUNCTION(BlueprintPure, Category="Target Sensing")
	inline int32 GetEnemyNumInBounds();
};