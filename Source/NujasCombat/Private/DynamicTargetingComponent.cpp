// Copyright 2018-2019 The Janus Project | 2034 Complex LLC. All Rights Reserved.

#include "DynamicTargetingComponent.h"
#include "Components/ArrowComponent.h"
#include "GameFramework/Character.h"
#include "Core/Public/Misc/App.h"
#include "ViewportUtility.h"
#include "Kismet/GameplayStatics.h"
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
		PlayerController = Cast<APlayerController>(Owner->GetController());
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

void UDynamicTargetingComponent::InitializeArrowComponent(UArrowComponent* const ArrowComp)
{
	ArrowComponent = ArrowComp;
	ArrowComponent->bAbsoluteRotation = true;
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
	PlayerController->SetControlRotation(LookAtRotation);
}

void UDynamicTargetingComponent::UpdateStrafeAssist()
{
	// collect all of the enemies on screen
	//ActorsOnScreen = FindAllActorsOnScreen();
	float SummarizedSpeed = 0.f;
	if(AActor* const ClosestActor = FindClosestTargetOnScreen())
	{
		if(IsTraceBlocked(ClosestActor))
			return;
		bool bSuccessfulProjection = false;
		// project their world space position into 2D screen space and take the X
		const FVector2D ActorScreenSpacePos = UViewportUtility::GetActorOnScreenPosition(ClosestActor, PlayerController, bSuccessfulProjection);
		const uint32& ActorID = ClosestActor->GetUniqueID();
		if(ActorHorizontalMovementMap.Contains(ActorID))
		{
			FVector2D ScreenSize;
			UViewportUtility::GetViewportSize(PlayerController, ScreenSize);
			const float SizeXFloated = ScreenSize.X / 2.f;

			FHorizontalActorMovementData& MovementData = ActorHorizontalMovementMap[ActorID];
			// if a valid entry in the map exists -> take the difference in the last known positions and save the speed
			const float CurrentSpeed = ActorScreenSpacePos.X - MovementData.LastKnownXPosition;
			
			const float CurrentOffset = FMath::Abs(SizeXFloated - ActorScreenSpacePos.X);
			const float PreviousOffset = FMath::Abs(SizeXFloated - MovementData.LastKnownXPosition);

			MovementData.LastKnownXPosition = ActorScreenSpacePos.X;
			MovementData.LastUpdatedXSpeed = CurrentSpeed;
			if(LastActorId == ActorID && CurrentOffset > PreviousOffset && FMath::Abs(CurrentSpeed) < 20.f)
			{
				// every speed that is not NAN and exists in the map will be accounted for and applied to the player's yaw
				SummarizedSpeed += CurrentSpeed;
			}
			else
			{
				LastActorId = ActorID;
			}
		}
		else
		{
			ActorHorizontalMovementMap.Add(ActorID, FHorizontalActorMovementData(ActorScreenSpacePos.X));
			LastActorId = ActorID;
		}
	}

	// instead of taking every character into account, take only one based on priority
	// How to determine the most important enemy
	// how far away is it from the player
	// how far away is it from the center of the screen
	// 
	// The camera should ignore an enemy unless the new position is moving away from the center
	// 
	// OR
	//
	// Collect the average screen space positions and interpolate towards it

	// apply the summarized rotation to the yaw of the player via interpolation
	FRotator ControllerRotation = PlayerController->GetControlRotation();
	ControllerRotation.Yaw = FMath::FInterpTo(ControllerRotation.Yaw, ControllerRotation.Yaw + SummarizedSpeed, FApp::GetDeltaTime(), 13.5f);
	PlayerController->SetControlRotation(ControllerRotation);
}

bool UDynamicTargetingComponent::IsTraceBlocked(const AActor* Target) const
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
	PlayerController->SetIgnoreLookInput(false);
}

void UDynamicTargetingComponent::EnableCameraLock()
{
	if (!SelectedActor->GetClass()->ImplementsInterface(UTargetable::StaticClass())) return;
	InvalidateStrafeAssist();
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
	PlayerController->SetIgnoreLookInput(true);
}

void UDynamicTargetingComponent::ToggleCameraLock()
{
	if (IsValidActorSelected())
	{
		DisableCameraLock();
	}
	else
	{
		SelectedActor = FindClosestTargetOnScreen();
		if (SelectedActor)
		{
			EnableCameraLock();
		}
	}
}

AActor* UDynamicTargetingComponent::FindClosestTargetOnScreen()
{
	AActor* FinalSelectedActor = nullptr;
	if (Owner)
	{
		ActorsOnScreen = FindAllActorsOnScreen();
		float DistanceFromCenterOfViewport = BIG_NUMBER;
		FVector2D ScreenSize;
		UViewportUtility::GetViewportSize(PlayerController, ScreenSize);
		const float SizeXFloated = ScreenSize.X / 2.f;
		for (AActor* const PotentialTarget : ActorsOnScreen)
		{
			bool bSuccess = false;
			const FVector2D ActorScreenPosition = UViewportUtility::GetActorOnScreenPosition(PotentialTarget, PlayerController, bSuccess);
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

TArray<AActor*> UDynamicTargetingComponent::FindAllActorsOnScreen()
{
	TArray<AActor*> FoundActors;
	TArray<AActor*> TargetableActors;
	UGameplayStatics::GetAllActorsWithInterface(Owner->GetWorld(), UTargetable::StaticClass(), TargetableActors);
	for (AActor*& TargetableActor : TargetableActors)
	{
		if (TargetableActor->GetClass()->ImplementsInterface(UTargetable::StaticClass()))
		{
			if (ITargetable::Execute_IsTargetable(TargetableActor))
			{
				bool bOnScreen = false;
				const float DistSquared = FVector::DistSquared(Owner->GetActorLocation(), TargetableActor->GetActorLocation());
				if(UViewportUtility::IsInViewport(UViewportUtility::GetActorOnScreenPosition(TargetableActor, PlayerController, bOnScreen), PlayerController)
					&& bOnScreen 
					&& DistSquared <= MaxDistanceToTargetSquared)
				{
					if (!IsTraceBlocked(TargetableActor))
					{
						FoundActors.Add(TargetableActor);
					}
				}
			}
		}
	}
	return FoundActors;
}

void UDynamicTargetingComponent::FindTargetWithAxisInput(float AxisInput)
{
	if(!ArrowComponent || !SelectedActor || FMath::Abs(AxisInput) < START_AXIS_THRESHOLD)
	{
		AxisCache = 0.f;
		return;
	}
	if(AxisCache * AxisInput > 0.f)
		return;
	AxisCache = AxisInput;
	TArray<AActor*> AllActors = FindAllActorsOnScreen();

	bool bSuccessfulProjection = false;
	AActor* FoundNextTarget = nullptr;
	const FVector2D OwnerScreenProjection = UViewportUtility::GetActorOnScreenPosition(Owner, PlayerController, bSuccessfulProjection);
	float ClosestEnemyDistance = NAN;
	float ClosestEnemyX = NAN;
	if(!bSuccessfulProjection) return;

	for (AActor*& PotentialTarget : AllActors)
	{

		if(PotentialTarget == SelectedActor)
			continue;
		const float DistSquared = FVector::DistSquared(PotentialTarget->GetActorLocation(), Owner->GetActorLocation());
		if(DistSquared > MaxDistanceToTargetSquared || DistSquared < MinDistanceToTargetSquared)
			continue;
		// no need to check if projection worked, because the target has already entered into the onscreen targets list
		const FVector2D PotentialTargetScreenPosition = UViewportUtility::GetActorOnScreenPosition(SelectedActor, PlayerController, bSuccessfulProjection);
		const float XOffset = PotentialTargetScreenPosition.X - OwnerScreenProjection.X;
		if(AxisInput * XOffset > 0.f)
		{
			if(FoundNextTarget)
			{
				if(DistSquared < ClosestEnemyDistance || FMath::Abs(XOffset) < FMath::Abs(ClosestEnemyX))
				{
					ClosestEnemyDistance = DistSquared;
					ClosestEnemyX = XOffset;
					FoundNextTarget = PotentialTarget;
				}
			}
			else
			{
				ClosestEnemyDistance = DistSquared;
				ClosestEnemyX = XOffset;
				FoundNextTarget = PotentialTarget;
			}
		}
	}
	if(FoundNextTarget)
	{
		DisableCameraLock();
		SelectedActor = FoundNextTarget;
		EnableCameraLock();
	}
}

void UDynamicTargetingComponent::ToggleStrafeAssist(bool bDecision)
{
	if(SelectedActor || !Owner)
	{
		InvalidateStrafeAssist();
		return;
	}
	if(bDecision && !StrafeAssistHandle.IsValid())
	{
		Owner->GetWorldTimerManager().SetTimer
		(
			StrafeAssistHandle, 
			this, 
			&UDynamicTargetingComponent::UpdateStrafeAssist, 
			0.01f, 
			true
		);
	}
	else if(!bDecision)
	{
		InvalidateStrafeAssist();
	}
}

void UDynamicTargetingComponent::InvalidateStrafeAssist()
{
	if(Owner)
	{
		Owner->GetWorldTimerManager().ClearTimer(StrafeAssistHandle);
		ActorHorizontalMovementMap.Empty();
		LastActorId = 0;
	}
}