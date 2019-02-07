// Copyright 2018-2019 The Janus Project | 2034 Complex LLC. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CharacterAnimationUtilityComponent.generated.h"

class ACharacter;
class UCharacterMovementComponent;

USTRUCT()
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

	//Cardinal Direction Args
	float RotationOffset;

	// TODO: Make Cardinal Direction Enum
	// TODO: Make Movement Direction Enum
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

  public:
	// Sets default values for this component's properties
	UCharacterAnimationUtilityComponent();

  protected:
	// Called when the game starts
	virtual void BeginPlay() override;

  public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

	/**
	 * Constraint the character rotation to always face the direction of the camera.
	 * Disables the rotation feature if the player controller supplies 0 for both inputs.
	 *
	 * @param VerticalInput			 The forward/backward float coming from the controller (W/S, Left Stick up and down)
	 * @param HorizontalInput		 The left/right float coming from the controller (A/D, Left Stick right left)
	 */
	UFUNCTION(BlueprintCallable, Category = "Character Animation Utility")
	void UpdateCharacterRotationBasedOnMovement(float VerticalInput, float HorizontalInput);

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
};
