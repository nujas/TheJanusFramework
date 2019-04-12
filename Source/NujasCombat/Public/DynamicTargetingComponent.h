#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/ActorComponent.h"
#include "Engine/EngineTypes.h"
#include "DynamicTargetingComponent.generated.h"

class UArrowComponent;
class ACharacter;
class UCharacterMovementComponent;

USTRUCT()
struct FRotationData
{
	GENERATED_BODY()

	uint8 bUseControllerRotationYaw : 1;
	uint8 bUseConrtollerDesiredRotation : 1;
	uint8 bOrientRotationToMovement : 1;
};

UINTERFACE(Blueprintable)
class NUJASCOMBAT_API UTargetable : public UInterface
{
	GENERATED_BODY()
};
class ITargetable
{
	GENERATED_BODY()
public:
	/* Call on Actor that got selected by the targeting component */
	UFUNCTION(BlueprintImplementableEvent, Category = "Dynamic Targeting")
	bool OnSelected();

	/* Call on Actor that got deselected by the targeting component */
	UFUNCTION(BlueprintImplementableEvent, Category = "Dynamic Targeting")
	bool OnDeselected();

	/* Call on Actor if you want to make sure that the actor is targetable */
	UFUNCTION(BlueprintImplementableEvent, Category = "Dynamic Targeting")
	bool IsTargetable();
};

UCLASS(ClassGroup = (NujasCombat))
class NUJASCOMBAT_API AMarker : public AActor
{
	GENERATED_BODY()

public:
	AMarker();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
};

/*
 * Component to aim the player at particular targets
 * For Debug purposes, it is desirable that the actor that owns this component
 *			 has an arrow component
 * TODO: Make the component assign an arrow at runtim if the specific qualifiers (toggles) are met
 */
UCLASS(ClassGroup = (NujasCombat), meta = (BlueprintSpawnableComponent))
class NUJASCOMBAT_API UDynamicTargetingComponent : public UActorComponent
{
	GENERATED_BODY()

	// Cache player rotation settings
	void SaveRotationSettings();
	// Restore player rotation settings once no longer targeting
	void RestoreRotationSettings();

	UPROPERTY()
	UCharacterMovementComponent* CharacterMovementComponent;
	UPROPERTY()
	UArrowComponent* ArrowComponent;
	UPROPERTY()
	AActor* SelectedActor;
	UPROPERTY()
	ACharacter* Owner;

	FRotationData PlayerRotationData;
	
	// Use this handle to periodically check if the target is still visible
	FTimerHandle TargetStillInSightHandle;

public:
	// Sets default values for this component's properties
	UDynamicTargetingComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision Channels")
	TArray<TEnumAsByte<EObjectTypeQuery>> ValidCollisionTraces; // Pawn by defualt
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision Channels")
	TArray<TEnumAsByte<EObjectTypeQuery>> BlockCollisionTraces;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
	void InitializeArrowComponent(UArrowComponent* const ArrowComponent);
	void DisableCameraLock();
	void EnableCameraLock();
	AActor* GetSelectedActor() const;
	bool IsValidActorSelected() const;
};