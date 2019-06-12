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

USTRUCT()
struct FHorizontalActorMovementData
{
	GENERATED_BODY()
	FHorizontalActorMovementData()
	{
		LastKnownXPosition = 0.f;
		LastUpdatedXSpeed = 0.f;
	};
	FHorizontalActorMovementData(float XPosition)
	{
		LastKnownXPosition = XPosition;
		LastUpdatedXSpeed = 0.f;
	}
	// NAN : the actor's data has been created but not yet calculated
	float LastKnownXPosition = NAN;
	float LastUpdatedXSpeed = NAN;
};

UINTERFACE(Blueprintable)
class NUJASCOMBAT_API UTargetable : public UInterface // TODO: needs to live inside the combat globals .h
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

/*
 * Component to aim the player at particular targets
 * For Debug purposes, it is desirable that the actor that owns this component
 *			 has an arrow component
 * 
 */

static const float MAX_DISTANCE_TO_TARGET_SQUARED = 6250000.f;
static const float MIN_DISTANCE_TO_TARGET_SQUARED = 2500.f;
static const float START_AXIS_THRESHOLD = 0.75f;
static const float ARROW_ROT_THRESHOLD = 165.f;

UCLASS(ClassGroup = (NujasCombat), meta = (BlueprintSpawnableComponent))
class NUJASCOMBAT_API UDynamicTargetingComponent : public UActorComponent
{
	GENERATED_BODY()

	// Cache player rotation settings
	inline void SaveRotationSettings();
	// Restore player rotation settings once no longer targeting
	inline void RestoreRotationSettings();
	// update the character rotation config data to face the targeted actor
	void UpdateFaceTargetConfig();
	// Check if the target blocked or dead. If true, simply disable targeting
	void CheckupTargetedActor();
	// retrieve the arrow component for debugging
	inline void InitializeArrowComponent(UArrowComponent* const ArrowComp);
	// call this "sudo" every frame to orientate the character camera towards the selected actor
	void UpdateCameraLock();
	// Use this to collect strafe data for the player to align the camera towards the enemies without moving the joystick while strafing
	void UpdateStrafeAssist();

	// detect if the currently targeted actor is blocked by some other object
	bool IsTraceBlocked(const AActor* Target) const;

	UPROPERTY()
	UCharacterMovementComponent* CharacterMovementComponent;
	UPROPERTY()
	UArrowComponent* ArrowComponent;
	UPROPERTY()
	AActor* SelectedActor;
	UPROPERTY()
	ACharacter* Owner;
	UPROPERTY()
	APlayerController* PlayerController;

	UPROPERTY()
	// Data that influences the character rotation via character movement component
	FRotationData PlayerRotationData;
	UPROPERTY()
	// Use this handle to periodically check if the target is still visible
	FTimerHandle TargetStillInSightHandle;
	UPROPERTY()
	// Handle responsible for updating the camera if there is a valid actor to look at
	FTimerHandle CameraLockUpdateHandle;
	UPROPERTY()
	// Utilize for updating the component strafe assist feature
	FTimerHandle StrafeAssistHandle;
	UPROPERTY()
	// Cache for on screen actors
	TArray<AActor*> ActorsOnScreen;

	UPROPERTY()
	// when you look through the strafed actors, it shouldn't matter if they are "targetable", you just want to keep them in sight
	TMap<uint32, FHorizontalActorMovementData> ActorHorizontalMovementMap;
	UPROPERTY()
	uint32 LastActorId = 0;

	UPROPERTY()
	float AxisCache = NAN;

public:
	// Sets default values for this component's properties
	UDynamicTargetingComponent();

	// TODO: I don't really have anything in store for Valid collisions. Maybe in the future we could use them for something
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision Channels")
	TArray<TEnumAsByte<EObjectTypeQuery>> ValidCollisionTraces;
	// Use these channels if the targeted actor is behind one of them
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision Channels")
	TArray<TEnumAsByte<EObjectTypeQuery>> BlockCollisionTraces;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dynamic Targeting Properties")
	float MaxDistanceToTargetSquared = MAX_DISTANCE_TO_TARGET_SQUARED;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dynamic Targeting Properties")
	float MinDistanceToTargetSquared = MIN_DISTANCE_TO_TARGET_SQUARED;
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:

	// Camera Lock
	UFUNCTION(BlueprintCallable, Category="Dynamic Targeting")
	void DisableCameraLock();
	UFUNCTION(BlueprintCallable, Category="Dynamic Targeting")
	void EnableCameraLock();
	UFUNCTION(BlueprintCallable, Category="Dynamic Targeting")
	void ToggleCameraLock();
	UFUNCTION(BlueprintCallable, Category="Dynamic Targeting")
	AActor* FindClosestTargetOnScreen();
	UFUNCTION(BlueprintCallable, Category="Dynamic Targeting")
	TArray<AActor*> FindAllActorsOnScreen();
	UFUNCTION(BlueprintCallable, Category="Dynamic Targeting")
	void FindTargetWithAxisInput(float AxisInput);

	//Strafe
	UFUNCTION(BlueprintCallable, Category="Dynamic Strafe Targeting")
	void ToggleStrafeAssist(bool bDecision); // Disallow toggling of strafe data if manual targeting is enabled
	UFUNCTION(BlueprintCallable, Category="Dynamic Strafe Targeting")
	void InvalidateStrafeAssist();

	// Getters
	UFUNCTION(BlueprintCallable, Category="Dynamic Targeting")
	inline AActor* GetSelectedActor() const { return SelectedActor; };
	UFUNCTION(BlueprintCallable, Category="Dynamic Targeting")
	inline bool IsValidActorSelected() const { return SelectedActor; };
};