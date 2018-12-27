// Fill out your copyright notice in the Description page of Project Settings.

#include "Animation/MontageUtilityComponent.h"

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

	owner = Cast<ACharacter>(GetOwner());
	ownerAnimInstance = owner->GetMesh()->GetAnimInstance();
	ownerMovement = owner->GetCharacterMovement();

	// ...

}


// Called every frame
void UMontageUtilityComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

float UMontageUtilityComponent::GetCurrentMontageTime()
{
	if (!owner->GetMesh() || ownerAnimInstance == nullptr)
	{
		return -1.f;
	}
	return ownerAnimInstance->Montage_GetPosition(owner->GetCurrentMontage());
}

float UMontageUtilityComponent::GetCurrentMontagePlayRate()
{
	if (!owner->GetMesh() || ownerAnimInstance == nullptr)
	{
		return -1.f;
	}
	return ownerAnimInstance->Montage_GetPlayRate(owner->GetCurrentMontage());
}

void UMontageUtilityComponent::SetCurrentMontagePlayRate(float rate)
{
	ownerAnimInstance->Montage_SetPlayRate(owner->GetCurrentMontage(), rate);
}

void UMontageUtilityComponent::EnableRotationOverride()
{
	ownerMovement->bAllowPhysicsRotationDuringAnimRootMotion = true;
}

void UMontageUtilityComponent::DisableRotationOverride()
{
	ownerMovement->bAllowPhysicsRotationDuringAnimRootMotion = false;
}

