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
	Owner = Cast<ACharacter>(GetOwner());
	OwnerMovement = Owner->GetCharacterMovement();
}

void UCharacterAnimationUtilityComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UCharacterAnimationUtilityComponent::UpdateCharacterRotationBasedOnMovement(float input)
{
	if (OwnerMovement)
	{
		if (input != 0)
		{
			OwnerMovement->bUseControllerDesiredRotation = true;
		}
		else
		{
			OwnerMovement->bUseControllerDesiredRotation = false;
		}
	}
}