// Copyright 2018-2019 The Janus Project | 2034 Complex LLC. All Rights Reserved.

#include "MontageUtilityComponent.h"

// Sets default values for this component's properties
UMontageUtilityComponent::UMontageUtilityComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	/*
	*/

	// ...
}

// Called when the game starts
void UMontageUtilityComponent::BeginPlay()
{
	Super::BeginPlay();

	Owner = Cast<ACharacter>(GetOwner());
	OwnerAnimInstance = Owner->GetMesh()->GetAnimInstance();
	OwnerMovement = Owner->GetCharacterMovement();

	// ...
}

// Called every frame
void UMontageUtilityComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

float UMontageUtilityComponent::GetCurrentMontageTime()
{
	if (!Owner->GetMesh() || OwnerAnimInstance == nullptr)
	{
		return -1.f;
	}
	return OwnerAnimInstance->Montage_GetPosition(Owner->GetCurrentMontage());
}

float UMontageUtilityComponent::GetCurrentMontagePlayRate()
{
	if (!Owner->GetMesh() || OwnerAnimInstance == nullptr)
	{
		return -1.f;
	}
	return OwnerAnimInstance->Montage_GetPlayRate(Owner->GetCurrentMontage());
}

void UMontageUtilityComponent::SetCurrentMontagePlayRate(float rate)
{
	OwnerAnimInstance->Montage_SetPlayRate(Owner->GetCurrentMontage(), rate);
}

void UMontageUtilityComponent::EnableRotationOverride()
{
	OwnerMovement->bAllowPhysicsRotationDuringAnimRootMotion = true;
}

void UMontageUtilityComponent::DisableRotationOverride()
{
	OwnerMovement->bAllowPhysicsRotationDuringAnimRootMotion = false;
}
