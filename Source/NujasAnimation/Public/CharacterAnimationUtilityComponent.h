// Copyright 2018-2019 The Janus Project | 2034 Complex LLC. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CharacterAnimationUtilityComponent.generated.h"

class ACharacter;
class UCharacterMovementComponent;

UENUM(BlueprintType)
enum class ECardinalDirection: uint8
{
	North			UMETA(DisplayName = "North"),
	East			UMETA(DisplayName = "East"),
	West			UMETA(DisplayName = "West"),
	South			UMETA(DisplayName = "South")
};

UENUM(BlueprintType)
enum class EMovementDirection: uint8
{
	Forward			UMETA(DisplayName = "Forward"),
	Backward		UMETA(DisplayName = "Backward")
};

USTRUCT(BlueprintType)
struct FAnimationUtilData
{
	GENERATED_BODY()
	// Grounded Args 
	float Direction;
	float MovementSpeed;

	// Air Time Args
	float FallSpeed;
	float LeanInAir;
	float InAirTime;

	// TODO: Make this openly editable
	float JumpVelocity = 100.0f;

	bool bCharacterIsMoving;
	bool bReceivingPlayerInput;

	FRotator VelocityRotator;

	ECardinalDirection CardinalDirection = ECardinalDirection::North;
	EMovementDirection MovementDirection = EMovementDirection::Forward;

	FVector4 MovementDirectionConstraints; // Only for Forward / Backward
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class NUJASANIMATION_API UCharacterAnimationUtilityComponent : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY(EditInstanceOnly)
	FAnimationUtilData AnimationUtilData;
	UPROPERTY(VisibleDefaultsOnly)
	ACharacter *CharacterOwner;
	UPROPERTY(VisibleDefaultsOnly)
	UCharacterMovementComponent *ChracterMovementComponent;

	void UpdateAnimationData();
	void UpdateInAirData();
	void SetCardinalEnum();
	void SetDirectionEnum();
	void SetCurrentSpeed();
	void SetCurrentDirection();

  public:
	// Sets default values for this component's properties
	UCharacterAnimationUtilityComponent();

  protected:
	// Called when the game starts
	virtual void BeginPlay() override;

  public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;


	UFUNCTION(BlueprintCallable, Category = "Character Animation Utility")
	void UpdateForwardBasedAnimationSystem(float VerticalInput, float HorizontalInput);

	/**
	 * Determine if character is moving.
	 * Based on current Actor Velocity.
	 */
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Character Animation Utility")
	bool IsCharacterMoving();

	/**
	 * Determine if the PC is still supplying movement input.
	 * Based on the last input vector.
	 */
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Character Animation Utility")
	bool IsThereMovementInput();

	/**
	 * Constraint the character rotation to always face the direction of the camera.
	 * Disables the rotation feature if the player controller supplies 0 for both inputs.
	 *
	 * @param VerticalInput			 The forward/backward float coming from the controller (W/S, Left Stick up and down)
	 * @param HorizontalInput		 The left/right float coming from the controller (A/D, Left Stick right left)
	 */
	void UpdateCharacterRotationBasedOnMovement(float VerticalInput, float HorizontalInput);

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Character Animation Utility")
	FAnimationUtilData GetAnimationUtilData() const;

	static bool IsFloatInDualRange(float TargetFloat, FVector4 DualRange, bool RangeSelect);
};
