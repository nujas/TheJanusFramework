// Copyright 2018-2019 The Janus Project | 2034 Complex LLC. All Rights Reserved.

#include "DynamicTargetingComponent.h"
#include "Components/ArrowComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

AMarker::AMarker()
{
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void AMarker::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AMarker::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void UDynamicTargetingComponent::SaveRotationSettings()
{
	if (Owner && CharacterMovementComponent)
	{
		PlayerRotationData.bUseControllerRotationYaw = Owner->bUseControllerRotationYaw;
		PlayerRotationData.bUseConrtollerDesiredRotation = CharacterMovementComponent->bUseControllerDesiredRotation;
		PlayerRotationData.bOrientRotationToMovement = CharacterMovementComponent->bOrientRotationToMovement;
	}
}

void UDynamicTargetingComponent::RestoreRotationSettings()
{
	if (Owner && CharacterMovementComponent)
	{
		Owner->bUseControllerRotationYaw = PlayerRotationData.bUseControllerRotationYaw;
		CharacterMovementComponent->bUseControllerDesiredRotation = PlayerRotationData.bUseConrtollerDesiredRotation;
		CharacterMovementComponent->bOrientRotationToMovement = PlayerRotationData.bOrientRotationToMovement;
	}
}

UDynamicTargetingComponent::UDynamicTargetingComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UDynamicTargetingComponent::BeginPlay()
{
	Super::BeginPlay();
	Owner = Cast<ACharacter>(this->GetOwner());
	if (Owner)
	{
		InitializeArrowComponent(Owner->FindComponentByClass<UArrowComponent>());
		CharacterMovementComponent = Owner->FindComponentByClass<UCharacterMovementComponent>();
	}
	// If Blocking and Valid Query arrays contain same members, blocking takes priority and removes members from Valid array
	for (const EObjectTypeQuery& ObjectTypeQuery : BlockCollisionTraces)
	{
		if (ValidCollisionTraces.Contains(ObjectTypeQuery))
		{
			ValidCollisionTraces.Remove(ObjectTypeQuery);
		}
	}
}

// Called every frame
void UDynamicTargetingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UDynamicTargetingComponent::InitializeArrowComponent(UArrowComponent* const ArrowComponent)
{
	if (ArrowComponent)
	{
		this->ArrowComponent = ArrowComponent;
		this->ArrowComponent->bAbsoluteRotation = 0x1;
	}
}

void UDynamicTargetingComponent::DisableCameraLock()
{
	if (!SelectedActor) return;

	ITargetable* TargetActor = Cast<ITargetable>(SelectedActor);
	if (TargetActor)
	{
		TargetActor->OnDeselected();
	}
	SelectedActor = nullptr;
	RestoreRotationSettings();
	if (!Owner) return;

	if(IsInGameThread())
	{
		Owner->GetWorldTimerManager().ClearTimer(TargetStillInSightHandle);
	}
	Owner->GetController()->SetIgnoreLookInput(false);
}

void UDynamicTargetingComponent::EnableCameraLock()
{
	SaveRotationSettings();
}

AActor* UDynamicTargetingComponent::GetSelectedActor() const
{
	return SelectedActor;
}

bool UDynamicTargetingComponent::IsValidActorSelected() const
{
	return SelectedActor;
}
