// Copyright 2018-2019 The Janus Project | 2034 Complex LLC. All Rights Reserved.

#include "DynamicTargetingComponent.h"
#include "Components/ArrowComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Core/Public/Misc/App.h"
#include "GameFramework/CharacterMovementComponent.h"

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
	// NO TICK! All the targeting is handled by timers
	PrimaryComponentTick.bCanEverTick = false;
}

void UDynamicTargetingComponent::BeginPlay()
{
	Super::BeginPlay();
	// retrieve dependencies
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
	// TODO: Interpolate the camera
}

void UDynamicTargetingComponent::InitializeArrowComponent(UArrowComponent* const ArrowComponent)
{
	this->ArrowComponent = ArrowComponent;
	this->ArrowComponent->bAbsoluteRotation = true;
}

void UDynamicTargetingComponent::UpdateFaceTargetConfig()
{
	if (Owner && CharacterMovementComponent)
	{
		Owner->bUseControllerRotationYaw = false;
		CharacterMovementComponent->bUseControllerDesiredRotation = true;
		CharacterMovementComponent->bOrientRotationToMovement = false;
	}
}

void UDynamicTargetingComponent::CheckupTargetedActor()
{
	if (SelectedActor->GetClass()->ImplementsInterface(UTargetable::StaticClass()))
	{
		if (IsTraceBlocked(SelectedActor) || !ITargetable::Execute_IsTargetable(SelectedActor))
		{
			DisableCameraLock();
		}
	}
	else // how did you select the actor in the first place if he doesn't implement the targetable interface?
	{
		DisableCameraLock();
	}
}

void UDynamicTargetingComponent::UpdateCameraLock()
{
	if (!SelectedActor || !Owner)
		return;

	// check the distance of the selected actor to the owner
	// if pass set the arrow's rotation and the controller's rotation
	const float DistSquared = FVector::DistSquared(Owner->GetActorLocation(), SelectedActor->GetActorLocation());
	if (DistSquared < MinDistanceToTargetSquared || DistSquared > MaxDistanceToTargetSquared)
	{
		DisableCameraLock();
		return;
	}
	if (ArrowComponent)
	{
		FRotator ArrowRotation = ArrowComponent->RelativeRotation;
		FRotator OwnerRotation = Owner->GetControlRotation();
		ArrowRotation = FMath::RInterpConstantTo(ArrowRotation, OwnerRotation, FApp::GetDeltaTime(), 250.f);
		ArrowComponent->SetWorldRotation(FRotator(0.f, ArrowRotation.Yaw, 0.f));
	}
	const FRotator ControlRotation = Owner->GetControlRotation(); // TODO: Use this in the future if you want the X/Y movable
	const FVector OwnerLocation = Owner->GetActorLocation();
	const FVector SelectedActorLocation = SelectedActor->GetActorLocation();

	FRotator LookAtRotation = (SelectedActorLocation - OwnerLocation).Rotation();
	LookAtRotation = FMath::RInterpConstantTo(ControlRotation, LookAtRotation, FApp::GetDeltaTime(), 300.f);

	Owner->GetController()->SetControlRotation(LookAtRotation);
}

bool UDynamicTargetingComponent::IsTraceBlocked(AActor* Target) const
{
	if(!(BlockCollisionTraces.Num() > 0))
		return false;
	if(const UWorld* World = Target->GetWorld())
	{
		const FCollisionObjectQueryParams QueryParams(BlockCollisionTraces);
		FHitResult OutHit;
		return World->LineTraceSingleByObjectType
		(
			OutHit, 
			Owner->GetActorLocation(), 
			Target->GetActorLocation(), 
			QueryParams, 
			FCollisionQueryParams(NAME_None, false, Owner)
		);
	}
	return false;
}

void UDynamicTargetingComponent::DisableCameraLock()
{
	if (!SelectedActor) return;

	if (SelectedActor->GetClass()->ImplementsInterface(UTargetable::StaticClass()))
	{
		ITargetable::Execute_OnDeselected(SelectedActor);
	}
	SelectedActor = nullptr;
	RestoreRotationSettings();
	if (!Owner) return;

	if(IsInGameThread())
	{
		Owner->GetWorldTimerManager().ClearTimer(CameraLockUpdateHandle);
		Owner->GetWorldTimerManager().ClearTimer(TargetStillInSightHandle);
	}
	Owner->GetController()->SetIgnoreLookInput(false);
}

void UDynamicTargetingComponent::EnableCameraLock()
{
	if (!SelectedActor->GetClass()->ImplementsInterface(UTargetable::StaticClass())) return;
	SaveRotationSettings();
	ITargetable::Execute_OnSelected(SelectedActor);
	UpdateFaceTargetConfig();
	if(IsInGameThread())
	{
		// loop the timer to make sure that the target is still in sight
		Owner->GetWorldTimerManager().SetTimer
		(
			TargetStillInSightHandle, 
			this, 
			&UDynamicTargetingComponent::CheckupTargetedActor, 
			0.15f, 
			true
		);
		Owner->GetWorldTimerManager().SetTimer
		(
			CameraLockUpdateHandle,
			this,
			&UDynamicTargetingComponent::UpdateCameraLock,
			0.01f,
			true
		);
	}
	Owner->GetController()->SetIgnoreLookInput(true);
}

void UDynamicTargetingComponent::ToggleCameraLock()
{
	if (IsValidActorSelected())
	{
		DisableCameraLock();
	}
	else
	{
		SelectedActor = FindTargetOnScreen();
		if (SelectedActor)
		{
			EnableCameraLock();
		}
	}
}

AActor* UDynamicTargetingComponent::FindTargetOnScreen()
{
	AActor* FinalSelectedActor = nullptr;
	if (Owner)
	{
		TArray<AActor*> TargetableActors;
		TArray<AActor*> PotentialTargets;
		UGameplayStatics::GetAllActorsWithInterface(Owner->GetWorld(), UTargetable::StaticClass(), TargetableActors);
		for (AActor* TargetableActor : TargetableActors)
		{
			if (TargetableActor->GetClass()->ImplementsInterface(UTargetable::StaticClass()))
			{
				if (ITargetable::Execute_IsTargetable(TargetableActor))
				{
					bool bOnScreen = false;
					const float DistSquared = FVector::DistSquared(Owner->GetActorLocation(), TargetableActor->GetActorLocation());
					if(IsInViewport(GetActorOnScreenPosition(TargetableActor, bOnScreen)) && bOnScreen && DistSquared <= MaxDistanceToTargetSquared)
					{
						if (!IsTraceBlocked(TargetableActor))
						{
							PotentialTargets.Add(TargetableActor);
						}
					}
				}
			}
		}
		float DistanceFromCenterOfViewport = BIG_NUMBER;
		int32 SizeX = -1;
		int32 SizeY = -1;
		if(const APlayerController* PC = Cast<APlayerController>(Owner->GetController()))
		{
			PC->GetViewportSize(SizeX, SizeY);
		}
		const float SizeXFloated = float(SizeX) / 2.f;
		for (AActor* const PotentialTarget : PotentialTargets)
		{
			bool bSuccess = false;
			const FVector2D ActorScreenPosition = GetActorOnScreenPosition(PotentialTarget, bSuccess);
			const float CurrentDistanceFromScreen = FMath::Abs(ActorScreenPosition.X - SizeXFloated);
			if (CurrentDistanceFromScreen < DistanceFromCenterOfViewport && bSuccess)
			{
				DistanceFromCenterOfViewport = CurrentDistanceFromScreen;
				FinalSelectedActor = PotentialTarget;
			}
		}
	}
	return FinalSelectedActor;
}

bool UDynamicTargetingComponent::IsInViewport(FVector2D Position) const
{
	if(APlayerController* PC = Cast<APlayerController>(Owner->GetController()))
	{
		const int32 PosX = Position.X;
		const int32 PosY = Position.Y;
		int32 SizeX = -1;
		int32 SizeY = -1;
		PC->GetViewportSize(SizeX, SizeY);
		return (PosX > 0 && PosY > 0 && PosX <= SizeX && PosY <= SizeY);
	}
	return false;
}

FVector2D UDynamicTargetingComponent::GetActorOnScreenPosition(AActor* Target, bool& bSuccess) const
{
	FVector2D ScreenPosition;
	bSuccess = UGameplayStatics::ProjectWorldToScreen(Cast<APlayerController>(Owner->GetController()), Target->GetActorLocation(), ScreenPosition);
	return ScreenPosition;
}

AActor* UDynamicTargetingComponent::GetSelectedActor() const
{
	return SelectedActor;
}

bool UDynamicTargetingComponent::IsValidActorSelected() const
{
	return SelectedActor;
}
