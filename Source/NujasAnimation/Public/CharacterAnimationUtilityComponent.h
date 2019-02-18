// Copyright 2018-2019 The Janus Project | 2034 Complex LLC. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CharacterAnimationUtilityComponent.generated.h"

class ACharacter;
class UCharacterMovementComponent;

UENUM(BlueprintType)
enum class ETargetingState : uint8
{
	Player			UMETA(DisplayName = "Player Control"),
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

	// Air Time Args
	UPROPERTY(BlueprintReadOnly)
	float FallSpeed;
	UPROPERTY(BlueprintReadOnly)
	float LeanInAir;
	UPROPERTY(BlueprintReadOnly)
	float InAirTime;

	UPROPERTY(BlueprintReadOnly)
	float JumpVelocity = 100.0f;

	UPROPERTY(BlueprintReadOnly)
	float LeanRotation;
	UPROPERTY(BlueprintReadOnly)
	float LeanAcceleration;

	UPROPERTY(BlueprintReadOnly)
	bool bCharacterIsMoving;
	UPROPERTY(BlueprintReadOnly)
	bool bReceivingPlayerInput;

	UPROPERTY(BlueprintReadOnly)
	FRotator VelocityRotator;

	//TODO: Cardinal Direction is no longer needed, but it's a good thing to look up for certain actions
	UPROPERTY(BlueprintReadOnly)
	ECardinalDirection CardinalDirection = ECardinalDirection::North;
	UPROPERTY(BlueprintReadOnly)
	EMovementDirection MovementDirection = EMovementDirection::Forward;

	UPROPERTY(BlueprintReadOnly)
	FVector4 MovementDirectionConstraint; // Only for Forward / Backward

	UPROPERTY(BlueprintReadOnly)
	FCardinalDirectionConstraint CardinalDirectionConstraint;
	
	UPROPERTY(BlueprintReadOnly)
	ETargetingState TargetingState = ETargetingState::Player;
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

	// Main function that all of the animation data present in the FAnimationUtilData struct
	void UpdateAnimationData();

	void UpdateInAirData();
	void SetCardinalEnum();
	void SetDirectionEnum();
	void SetCurrentSpeed();
	void SetCurrentDirection();
	void UpdateCharacterLeanRotation();
	void UpdateCharacterLeanAcceleration();

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
	ETargetingState GetTargetingState() const;

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Character Animation Utility")
	FAnimationUtilData GetAnimationUtilData() const;

	UFUNCTION(BlueprintCallable, Category = "Character Animation Utility")
	void SetTargetingState(ETargetingState TargetingState);
};
