// Copyright 2018-2019 The Janus Project | 2034 Complex LLC. All Rights Reserved.

#include "CharacterAnimationUtilityComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

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
		if (!FMath::IsNearlyZero(VerticalInput, 0.01f) || !FMath::IsNearlyZero(HorizontalInput, 0.01f))
		{
			ChracterMovementComponent->bUseControllerDesiredRotation = true;
		}
		else
		{
			ChracterMovementComponent->bUseControllerDesiredRotation = false;
		}
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
	if(CharacterOwner)
		return CharacterOwner->GetVelocity().IsNearlyZero(0.01f) ? false : true;
	return false;
}

bool UCharacterAnimationUtilityComponent::IsThereMovementInput()
{
	if (ChracterMovementComponent)
		return ChracterMovementComponent->GetLastInputVector().IsNearlyZero(0.001f) ? false : true;
	return false;
}
