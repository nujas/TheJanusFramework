// Copyright 2018-2019 The Janus Project | 2034 Complex LLC. All Rights Reserved.

#include "CharacterAnimationUtilityComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Runtime/Core/Public/Misc/App.h"

UCharacterAnimationUtilityComponent::UCharacterAnimationUtilityComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UCharacterAnimationUtilityComponent::BeginPlay()
{
	Super::BeginPlay();
	CharacterOwner = Cast<ACharacter>(GetOwner());
	ChracterMovementComponent = CharacterOwner->GetCharacterMovement();
}

void UCharacterAnimationUtilityComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UCharacterAnimationUtilityComponent::UpdateCharacterRotationBasedOnMovement(float VerticalInput, float HorizontalInput)
{
	if (ChracterMovementComponent)
	{
		ChracterMovementComponent->bUseControllerDesiredRotation =
			!FMath::IsNearlyZero(VerticalInput, 0.01f) ||
			!FMath::IsNearlyZero(HorizontalInput, 0.01f);
	}
	else
	{
		UE_LOG
		(
			LogTemp,
			Error,
			TEXT("The Owner's movement component could not be accessed in %s"),
			*this->GetName()
		);
	}
}

bool UCharacterAnimationUtilityComponent::IsCharacterMoving()
{
	if (CharacterOwner)
		return !CharacterOwner->GetVelocity().IsNearlyZero(0.01f);
	return false;
}

bool UCharacterAnimationUtilityComponent::IsThereMovementInput()
{
	if (ChracterMovementComponent)
		return !ChracterMovementComponent->GetLastInputVector().IsNearlyZero(0.001f);
	return false;
}

void UCharacterAnimationUtilityComponent::UpdateForwardBasedAnimationSystem(float VerticalInput, float HorizontalInput)
{
	UpdateCharacterRotationBasedOnMovement(VerticalInput, HorizontalInput);
	UpdateAnimationData();
}

void UCharacterAnimationUtilityComponent::UpdateAnimationData()
{
	SetCurrentSpeed();
	UpdateInAirData();
	AnimationUtilData.bCharacterIsMoving = IsCharacterMoving();
	AnimationUtilData.bReceivingPlayerInput = IsThereMovementInput();
}

void UCharacterAnimationUtilityComponent::UpdateInAirData()
{
	if(ChracterMovementComponent)
	{
		if (ChracterMovementComponent->IsFalling())
		{
			if (CharacterOwner)
			{
				float FallSpeed = CharacterOwner->GetVelocity().Z;
				float JumpVelocity = AnimationUtilData.JumpVelocity;
				AnimationUtilData.FallSpeed = FallSpeed;
				AnimationUtilData.InAirTime += FApp::GetDeltaTime();
				float ClampedFallSpeed = FMath::GetMappedRangeValueClamped
				(
					{ JumpVelocity, -1 * JumpVelocity },
					{ 1, -1 },
					FallSpeed
				);
				float NormalizedInRangeSpeed = FMath::Clamp
				(
					AnimationUtilData.MovementSpeed,
					0.f,
					ChracterMovementComponent->MaxWalkSpeed
				);
				AnimationUtilData.LeanInAir = ClampedFallSpeed * NormalizedInRangeSpeed;
			}
		}
		else
		{
			AnimationUtilData.InAirTime = 0;
			AnimationUtilData.FallSpeed = 0;
			AnimationUtilData.LeanInAir = 0;
		}
	}
}

void UCharacterAnimationUtilityComponent::SetCurrentSpeed()
{
	if (CharacterOwner)
	{
		AnimationUtilData.MovementSpeed = CharacterOwner->GetVelocity().Size();
	}
}

void UCharacterAnimationUtilityComponent::SetCurrentDirection()
{
	// TODO: Rotations Calc Necessary in order to correctly predict direction
}
