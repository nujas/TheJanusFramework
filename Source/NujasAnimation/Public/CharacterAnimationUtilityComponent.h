// Copyright 2018-2019 The Janus Project | 2034 Complex LLC. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CharacterAnimationUtilityComponent.generated.h"

class ACharacter;
class UCharacterMovementComponent;
class UCapsuleComponent;

UENUM(BlueprintType)
enum class ETargetingState : uint8
{
	Manual			UMETA(DisplayName = "Manual"),
	AutoAim			UMETA(DisplayName = "AutoAim"),
	Cinematic		UMETA(DisplayName = "Cinematic")
};

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
struct FCardinalDirectionConstraint
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly)
	FVector4 SouthConstraint = FVector4(130.f, 180.f, -180.f, -130.f);
	UPROPERTY(BlueprintReadOnly)
	FVector4 WestConstraint = FVector4(-140.f, -40.f, -130.f, 50.f);
	UPROPERTY(BlueprintReadOnly)
	FVector4 EastConstraint = FVector4(40.f, 140.f, 50.f, 130.f);
	UPROPERTY(BlueprintReadOnly)
	FVector4 NorthConstraint = FVector4(-50.f, 50.f, -40.f, 40.f);
};

USTRUCT(BlueprintType)
struct FAnimationUtilData
{
	GENERATED_BODY()

	// Grounded Args 
	UPROPERTY(BlueprintReadOnly)
	float Direction;
	UPROPERTY(BlueprintReadOnly)
	float MovementSpeed;
	UPROPERTY(BlueprintReadOnly)
	float DeltaCharacterVelocity;
	UPROPERTY(BlueprintReadOnly)
	float FootStartPosition; // in the animation sequence, not a literal 3D space position
	// term that is used for the manner of walking. In general there should be a max of 3 (Walking, Running/Jogging, Sprinting)
	UPROPERTY(BlueprintReadOnly)
	float AnimationGait; 
	UPROPERTY(BlueprintReadOnly)
	float AnimationPlayRate;

	// Air Time Args
	UPROPERTY(BlueprintReadOnly)
	float FallSpeed;
	UPROPERTY(BlueprintReadOnly)
	float LeanInAir;
	UPROPERTY(BlueprintReadOnly)
	float InAirTime;
	UPROPERTY(BlueprintReadOnly)
	float JumpVelocity = 100.0f;

	// Aiming Args
	UPROPERTY(BlueprintReadOnly)
	float AimYawDelta;
	UPROPERTY(BlueprintReadOnly)
	float AimYawRate;

	// Leaning Args
	UPROPERTY(BlueprintReadOnly)
	float LeanRotation;
	UPROPERTY(BlueprintReadOnly)
	float LeanAcceleration;
	UPROPERTY(BlueprintReadOnly)
	FVector2D Lean; // A combined rotation and acceleration lean around the -1 Z axis


	// Player Input / character movement detectors
	UPROPERTY(BlueprintReadOnly)
	bool bCharacterIsMoving;
	UPROPERTY(BlueprintReadOnly)
	bool bReceivingPlayerInput;

	UPROPERTY(BlueprintReadOnly)
	FRotator VelocityRotator;
	UPROPERTY(BlueprintReadOnly)
	FRotator MovementInputRotator; // movement input vector converted into a rotator
	UPROPERTY(BlueprintReadOnly)
	FRotator ControllerRotator; // Rotation Coming from the Controller input

	//TODO: Cardinal Direction is no longer needed, but it's a good thing to look up for certain actions
	UPROPERTY(BlueprintReadOnly)
	ECardinalDirection CardinalDirection = ECardinalDirection::North;
	UPROPERTY(BlueprintReadOnly)
	EMovementDirection MovementDirection = EMovementDirection::Forward;

	UPROPERTY(BlueprintReadOnly)
	FVector4 MovementDirectionConstraint; // Only for Forward / Backward

	UPROPERTY(BlueprintReadOnly)
	FVector2D AimOffset = FVector2D(0.f, 0.f); // Only For head rotations

	UPROPERTY(BlueprintReadOnly)
	FCardinalDirectionConstraint CardinalDirectionConstraint;
	
	UPROPERTY(BlueprintReadOnly)
	ETargetingState TargetingState = ETargetingState::Manual;
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class NUJASANIMATION_API UCharacterAnimationUtilityComponent : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY(EditInstanceOnly)
	FAnimationUtilData AnimationUtilData;
	UPROPERTY(VisibleDefaultsOnly)
	ACharacter* CharacterOwner;
	UPROPERTY(VisibleDefaultsOnly)
	UCharacterMovementComponent* ChracterMovementComponent;
	UPROPERTY(VisibleDefaultsOnly)
	UCapsuleComponent* CapsuleComponent;
	// Main function that all of the animation data present in the FAnimationUtilData struct
	void UpdateAnimationData();

	void SetCardinalEnum();
	void UpdateDirectionEnum(const float& DirectionMin = -90.f, const float& DirectionMax = 90.f, const float& Buffer = 5.f);
	void SetCurrentDirection();
	void UpdateInAirData();
	float UpdateCharacterLeanRotation();
	float UpdateCharacterLeanAcceleration();
	void UpdateCharacterAcceleration(const float& AccelerationMultiplier = 1000.f, const float& GroundFrictionMultiplier = 6.f);
	void UpdateAimData();
	void UpdateFootStartPosition(float& /*OUT*/ FootPosition, const float& LeftStart = 0.3f, const float& RightStart = 0.867f);
	void UpdateAnimationGait();
	void UpdateAnimationPlayRate(float& /*OUT*/ PlayRate);
	/**
	 * Constraint the character rotation to always face the direction of the camera.
	 * Disables the rotation feature if the player controller supplies 0 for both inputs.
	 *
	 * @param VerticalInput			 The forward/backward float coming from the controller (W/S, Left Stick up and down)
	 * @param HorizontalInput		 The left/right float coming from the controller (A/D, Left Stick right left)
	 */
	void UpdateCharacterRotationBasedOnMovement(const float VerticalInput, const float HorizontalInput);

  public:
	// Sets default values for this component's properties
	UCharacterAnimationUtilityComponent();
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = PlayerMovementConfig)
	float WalkSpeed = 200.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = PlayerMovementConfig)
	float RunSpeed = 400.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = PlayerMovementConfig)
	float SprintSpeed = 800.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = PlayerMovementConfig)
	float LeftFootStartPosition = 0.3f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = PlayerMovementConfig)
	float RightFootStartPosition = 0.867f;
  protected:
	// Called when the game starts
	virtual void BeginPlay() override;

  public:
	/**
	 * Publicly exposed method that will calculate all the necessary data from the character
	 * And store into the AnimUtilData struct
	 *
	 * @param VerticalInput			 The forward/backward float coming from the controller (W/S, Left Stick up and down)
	 * @param HorizontalInput		 The left/right float coming from the controller (A/D, Left Stick right left)
	 */
	UFUNCTION(BlueprintCallable, Category = "Character Animation Utility")
	void UpdateForwardBasedAnimationSystem(const float VerticalInput, const float HorizontalInput);

	/**
	 * Determine if character is moving.
	 * Based on current Actor Velocity.
	 */
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Character Animation Utility")
	bool IsCharacterMoving() const;

	/**
	 * Determine if the PC is still supplying movement input.
	 * Based on the last input vector.
	 */
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Character Animation Utility")
	bool IsThereMovementInput();

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Character Animation Utility")
	ETargetingState GetTargetingState() const;

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Character Animation Utility")
	FAnimationUtilData GetAnimationUtilData() const;

	UFUNCTION(BlueprintCallable, Category = "Character Animation Utility")
	void SetTargetingState(ETargetingState TargetingState);
};
