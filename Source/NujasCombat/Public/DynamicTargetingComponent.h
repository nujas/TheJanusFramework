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
	float LastKnownXPosition = NAN;
	float LastUpdatedXSpeed = NAN;
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

/*
 * Component to aim the player at particular targets
 * For Debug purposes, it is desirable that the actor that owns this component
 *			 has an arrow component
 * 
 */

UCLASS(ClassGroup = (NujasCombat), meta = (BlueprintSpawnableComponent))
class NUJASCOMBAT_API UDynamicTargetingComponent : public UActorComponent
{
	GENERATED_BODY()

	// Cache player rotation settings
	inline void SaveRotationSettings();
	// Restore player rotation settings once no longer targeting
	inline void RestoreRotationSettings();
	// retrieve the arrow component for debugging
	inline void InitializeArrowComponent(UArrowComponent* const ArrowComponent);
	// update the character rotation config data to face the targeted actor
	void UpdateFaceTargetConfig();
	// Check if the target blocked or dead. If true, simply disable targeting
	void CheckupTargetedActor();

	// call this "sudo" every frame to orientate the character camera towards the selected actor
	void UpdateCameraLock();
	// Use this to collect strafe data for the player to align the camera towards the enemies without moving the joystick while strafing
	void UpdateStrafeAssist();

	// detect if the currently targeted actor is blocked by some other object
	bool IsTraceBlocked(AActor* Target) const;
	// detect if the provided 2D location is within the viewport bounds
	bool IsInViewport(FVector2D Position) const; // TODO: Move to Util?
	// project the actor's V3 into a V2 on screen positions
	FVector2D GetActorOnScreenPosition(AActor* Target, bool& bSuccess) const; // TODO: Move to Util?

	UPROPERTY()
	UCharacterMovementComponent* CharacterMovementComponent;
	UPROPERTY()
	UArrowComponent* ArrowComponent;
	UPROPERTY()
	AActor* SelectedActor;
	UPROPERTY()
	ACharacter* Owner;

	// Data that influences the character rotation via character movement component
	FRotationData PlayerRotationData;
	// Use this handle to periodically check if the target is still visible
	FTimerHandle TargetStillInSightHandle;
	// Handle responsible for updating the camera if there is a valid actor to look at
	FTimerHandle CameraLockUpdateHandle;

	// Cache for on screen actors
	TArray<AActor*> ActorsOnScreen;

	// when you look through the strafed actors, it shouldn't matter if they are "targetable", you just want to keep them in sight
	TMap<FName, FHorizontalActorMovementData> ActorHorizontalMovementMap;
	bool bReceiveStrafeData;
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
	float MaxDistanceToTargetSquared = 6250000.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dynamic Targeting Properties")
	float MinDistanceToTargetSquared = 2500.f;
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
	
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
	void FindAllActorsOnScreen(TArray<AActor*>& OutActors);

	//Strafe
	UFUNCTION(BlueprintCallable, Category="Dynamic Strafe Targeting")
	inline void SetReceiveStrafeData(bool bDecision) { if(!SelectedActor) bReceiveStrafeData = bDecision; }; // Disallow toggling of strafe data if manual targeting is enabled

	// Getters
	UFUNCTION(BlueprintCallable, Category="Dynamic Targeting")
	inline AActor* GetSelectedActor() const { return SelectedActor; };
	UFUNCTION(BlueprintCallable, Category="Dynamic Targeting")
	inline bool IsValidActorSelected() const { return SelectedActor; };
};