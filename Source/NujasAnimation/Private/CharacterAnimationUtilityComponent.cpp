// Copyright 2018-2019 The Janus Project | 2034 Complex LLC. All Rights Reserved.

#include "CharacterAnimationUtilityComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Core/Public/Misc/App.h"
#include "MathUtility.h" // NujasCore Module

UCharacterAnimationUtilityComponent::UCharacterAnimationUtilityComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	AnimationUtilData.MovementDirectionConstraint = FVector4(-100, 100, -80, 80); // Default
}

void UCharacterAnimationUtilityComponent::BeginPlay()
{
	Super::BeginPlay();
	CharacterOwner = Cast<ACharacter>(GetOwner());
	ChracterMovementComponent = CharacterOwner->GetCharacterMovement();
	CapsuleComponent = CharacterOwner->FindComponentByClass<UCapsuleComponent>();
}

void UCharacterAnimationUtilityComponent::UpdateCharacterRotationBasedOnMovement(const float VerticalInput, const float HorizontalInput)
{
	if (ChracterMovementComponent)
	{
		ChracterMovementComponent->bUseControllerDesiredRotation =
			(!FMath::IsNearlyZero(VerticalInput, 0.01f) ||
			!FMath::IsNearlyZero(HorizontalInput, 0.01f) || 
			!FMath::IsNearlyEqual(ChracterMovementComponent->GetLastUpdateVelocity().Size(), 0.0f))
			&&
			AnimationUtilData.TargetingState == ETargetingState::Manual;
	}
#if WITH_EDITORONLY_DATA
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
#endif
}

bool UCharacterAnimationUtilityComponent::IsCharacterMoving() const
{
	if (CharacterOwner)
		return !CharacterOwner->GetVelocity().IsNearlyZero(0.01f);
	return false;
}

bool UCharacterAnimationUtilityComponent::IsThereMovementInput()
{
	if (ChracterMovementComponent)
	{
		const FVector LastInputVector = ChracterMovementComponent->GetLastInputVector();
		AnimationUtilData.MovementInputRotator = LastInputVector.Rotation();
		return !LastInputVector.IsNearlyZero(0.001f);
	}
	return false;
}

void UCharacterAnimationUtilityComponent::UpdateForwardBasedAnimationSystem(const float VerticalInput, const float HorizontalInput)
{
	UpdateCharacterRotationBasedOnMovement(VerticalInput, HorizontalInput);
	UpdateAnimationData();
}

void UCharacterAnimationUtilityComponent::UpdateAnimationData()
{
	AnimationUtilData.bCharacterIsMoving = IsCharacterMoving();
	AnimationUtilData.bReceivingPlayerInput = IsThereMovementInput();
	AnimationUtilData.LeanRotation = UpdateCharacterLeanRotation();
	AnimationUtilData.LeanAcceleration = UpdateCharacterLeanAcceleration();

	SetCurrentDirection();

	// Set Lean values + the lean struct
	FVector RotVector(AnimationUtilData.LeanRotation, AnimationUtilData.LeanAcceleration, 0.f);
	// Rotate around the downward unit vector
	RotVector = RotVector.RotateAngleAxis(AnimationUtilData.Direction, FVector(0.f, 0.f, -1.f)); 
	AnimationUtilData.Lean = FVector2D(RotVector.X, RotVector.Y);

	UpdateCharacterAcceleration();
	UpdateAimData(); // head only
	UpdateInAirData();
	UpdateAnimationGait();
	UpdateAnimationPlayRate(AnimationUtilData.AnimationPlayRate);
	
	SetCardinalEnum();
	UpdateDirectionEnum();

	if (AnimationUtilData.bCharacterIsMoving)
		UpdateFootStartPosition(AnimationUtilData.FootStartPosition, LeftFootStartPosition, RightFootStartPosition);
}

void UCharacterAnimationUtilityComponent::UpdateInAirData()
{
	if(ChracterMovementComponent)
	{
		if (ChracterMovementComponent->IsFalling())
		{
			if (CharacterOwner)
			{
				const float FallSpeed = CharacterOwner->GetVelocity().Z;
				const float JumpVelocity = AnimationUtilData.JumpVelocity;
				const float ClampedFallSpeed = FMath::GetMappedRangeValueClamped
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
				AnimationUtilData.FallSpeed = FallSpeed;
				AnimationUtilData.InAirTime += FApp::GetDeltaTime();
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

float UCharacterAnimationUtilityComponent::UpdateCharacterLeanRotation()
{
	if (!ChracterMovementComponent) 
		return 0.f;
	const float DeltaTime = FApp::GetDeltaTime();
	const FRotator VelocityRotator = CharacterOwner->GetVelocity().Rotation();
	const FRotator StoredRotator = AnimationUtilData.VelocityRotator.ContainsNaN() ? VelocityRotator : AnimationUtilData.VelocityRotator;

	FRotator VelocityRotatorDelta = VelocityRotator - StoredRotator;
	VelocityRotatorDelta.Normalize();

	float VelocityDelta = VelocityRotatorDelta.Yaw / DeltaTime;
	AnimationUtilData.VelocityRotator = VelocityRotator; // save

	VelocityDelta = FMath::GetMappedRangeValueClamped
	(
		FVector2D(-200.f, 200.f), 
		FVector2D(-1.f, 1.f),
		VelocityDelta
	);
	VelocityDelta *= FMath::GetMappedRangeValueClamped
	(
		FVector2D(WalkSpeed, RunSpeed), 
		FVector2D(0.f, 1.f), 
		AnimationUtilData.MovementSpeed
	);
	return VelocityDelta;
}

float UCharacterAnimationUtilityComponent::UpdateCharacterLeanAcceleration()
{
	// TODO: should play around with the interploation speed. 
	if (!ChracterMovementComponent)
		return 0.f;
	const float DeltaTime = FApp::GetDeltaTime();
	const float Speed = ChracterMovementComponent->Velocity.Size();
	const float SpeedFrameDif = Speed - AnimationUtilData.MovementSpeed;

	AnimationUtilData.MovementSpeed = Speed;
	const float AccelerationFrameDif = SpeedFrameDif / DeltaTime;

	float LeanAcceleration = 0.f;
	const float MaxAcceleration = ChracterMovementComponent->GetMaxAcceleration();
	const float BrakingAcceleration = ChracterMovementComponent->BrakingDecelerationWalking;
	if (AccelerationFrameDif > 0.f)
	{
		LeanAcceleration = FMath::GetMappedRangeValueClamped
		(
			FVector2D(0.f, MaxAcceleration), 
			FVector2D(0.f, 1.f), 
			FMath::Abs(AccelerationFrameDif)
		);
	}
	else
	{
		LeanAcceleration = FMath::GetMappedRangeValueClamped
		(
			FVector2D(0.f, BrakingAcceleration),
			FVector2D(0.f, -1.f),
			FMath::Abs(AccelerationFrameDif)
		);
	}
	LeanAcceleration *= FMath::GetMappedRangeValueClamped
	(
		FVector2D(WalkSpeed, RunSpeed), 
		FVector2D(0.f, 1.f), 
		Speed
	);
	return LeanAcceleration;
}

void UCharacterAnimationUtilityComponent::UpdateCharacterAcceleration(const float& AccelerationMultiplier, const float& GroundFrictionMultiplier)
{
	if (AnimationUtilData.VelocityRotator.ContainsNaN() 
		|| AnimationUtilData.MovementInputRotator.ContainsNaN() 
		|| !ChracterMovementComponent)
		return;

	FRotator DeltaRotator = AnimationUtilData.MovementInputRotator - AnimationUtilData.VelocityRotator;
	DeltaRotator.Normalize();
	float DeltaCharacterVelocity = DeltaRotator.Yaw;
	AnimationUtilData.DeltaCharacterVelocity = DeltaCharacterVelocity;
	DeltaCharacterVelocity = FMath::Abs(DeltaCharacterVelocity);
	DeltaCharacterVelocity = FMath::GetMappedRangeValueClamped
	(
		FVector2D(45.f, 130.f), 
		FVector2D(1.f, 0.2f), 
		DeltaCharacterVelocity
	);
	const float FrictionClamp = FMath::GetMappedRangeValueClamped
	(
		FVector2D(45.f, 130.f), 
		FVector2D(1.f, 0.4f), 
		DeltaCharacterVelocity
	);
	ChracterMovementComponent->MaxAcceleration = DeltaCharacterVelocity * AccelerationMultiplier;
	ChracterMovementComponent->GroundFriction = FrictionClamp * GroundFrictionMultiplier;
}

void UCharacterAnimationUtilityComponent::UpdateAimData()
{
	if (!CharacterOwner)
		return;
	const FRotator ControllerRotator = CharacterOwner->GetController()->GetControlRotation();
	if (AnimationUtilData.ControllerRotator.ContainsNaN())
	{
		AnimationUtilData.ControllerRotator = ControllerRotator;
	}
	const float ControllerYaw = AnimationUtilData.ControllerRotator.Yaw;
	AnimationUtilData.ControllerRotator = ControllerRotator;
	AnimationUtilData.AimYawRate = (ControllerRotator.Yaw - ControllerYaw) / FApp::GetDeltaTime();
	FRotator RotationDelta = ControllerRotator - CharacterOwner->GetActorRotation();
	RotationDelta.Normalize();
	AnimationUtilData.AimYawDelta = RotationDelta.Yaw;
	const float AimOffsetInterpSpeed = FMath::Abs(RotationDelta.Yaw - AnimationUtilData.AimOffset.X);
	const float ClampedAimOffsetInterpSpeed = FMath::GetMappedRangeValueClamped
	(
		FVector2D(0.f, 180.f), 
		FVector2D(8.f, 5.f), 
		AimOffsetInterpSpeed
	);
	if (FMath::Abs(AnimationUtilData.AimYawDelta) > 105.f)
	{
		AnimationUtilData.AimOffset = FMath::Vector2DInterpTo
		(
			AnimationUtilData.AimOffset, 
			FVector2D(0.f, 0.f), 
			FApp::GetDeltaTime(), 
			4.f
		);
	}
	else
	{
		const FVector2D TargetAimOffset(RotationDelta.Yaw, RotationDelta.Pitch);
		AnimationUtilData.AimOffset = FMath::Vector2DInterpTo
		(
			AnimationUtilData.AimOffset, 
			TargetAimOffset, 
			FApp::GetDeltaTime(), 
			ClampedAimOffsetInterpSpeed
		);
	}
}

// Simple update function to determine which foot to place (left or right)
void UCharacterAnimationUtilityComponent::UpdateFootStartPosition(float& FootPosition, const float& LeftStart, const float& RightStart)
{
	FootPosition = AnimationUtilData.Direction > 0 ? LeftStart : RightStart;
}

void UCharacterAnimationUtilityComponent::UpdateAnimationGait()
{
	const float MovementSpeed = AnimationUtilData.MovementSpeed;
	float Gait = 0.f;
	if (MovementSpeed <= WalkSpeed)
	{
		Gait = FMath::GetMappedRangeValueClamped
		(
			FVector2D(0.f, WalkSpeed), 
			FVector2D(0.f, 1.f), 
			MovementSpeed
		);
	}
	else if(MovementSpeed > WalkSpeed && MovementSpeed <= RunSpeed)
	{
		Gait = FMath::GetMappedRangeValueClamped
		(
			FVector2D(WalkSpeed, RunSpeed), 
			FVector2D(1.f, 2.f), 
			MovementSpeed
		);
	}
	else
	{
		Gait = FMath::GetMappedRangeValueClamped
		(
			FVector2D(RunSpeed, SprintSpeed), 
			FVector2D(2.f, 3.f), 
			MovementSpeed
		);
	}
	AnimationUtilData.AnimationGait = Gait;
}

void UCharacterAnimationUtilityComponent::UpdateAnimationPlayRate(float& OutPlayRate)
{
	const float Gait = AnimationUtilData.AnimationGait;
	const float Speed = AnimationUtilData.MovementSpeed;
	const float WalkUnclamped = FMath::GetMappedRangeValueUnclamped(FVector2D(0.f, WalkSpeed), FVector2D(0.f, 1.f), Speed);
	const float RunUnclamped = FMath::GetMappedRangeValueUnclamped(FVector2D(0.f, RunSpeed), FVector2D(0.f, 1.f), Speed);
	const float SprintUnclamped = FMath::GetMappedRangeValueUnclamped(FVector2D(0.f, SprintSpeed), FVector2D(0.f, 1.f), Speed);
	
	float ClampedPlayRate;
	if (Gait < 2.f)
	{
		ClampedPlayRate = FMath::GetMappedRangeValueClamped
		(
			FVector2D(1.f, 2.f), 
			FVector2D(WalkUnclamped, RunUnclamped), 
			Gait
		);
	}
	else
	{
		ClampedPlayRate = FMath::GetMappedRangeValueClamped
		(
			FVector2D(2.f, 3.f), 
			FVector2D(RunUnclamped, SprintUnclamped), 
			Gait
		);
	}
	OutPlayRate = ClampedPlayRate / CapsuleComponent->RelativeScale3D.Z;
}

void UCharacterAnimationUtilityComponent::SetCardinalEnum()
{
	if(IsCharacterMoving())
	{
		const FCardinalDirectionConstraint CDConstraint = AnimationUtilData.CardinalDirectionConstraint;
		const float LocalDirection = AnimationUtilData.Direction;
		const ECardinalDirection CurrentCardinalDirection = AnimationUtilData.CardinalDirection;
		ECardinalDirection FinalCardinalDirection = ECardinalDirection::North;
		bool bPass = UMathUtility::IsFloatInDualRange(LocalDirection, CDConstraint.EastConstraint, LocalDirection > 0.0f);
		FinalCardinalDirection = bPass ? ECardinalDirection::South : ECardinalDirection::North;
		bPass = UMathUtility::IsFloatInDualRange
		(
			LocalDirection, 
			CDConstraint.WestConstraint, 
			CurrentCardinalDirection == ECardinalDirection::West
		);
		FinalCardinalDirection = bPass ? ECardinalDirection::West : FinalCardinalDirection;
		bPass = UMathUtility::IsFloatInDualRange
		(
			LocalDirection, 
			CDConstraint.EastConstraint, 
			CurrentCardinalDirection == ECardinalDirection::East
		);
		FinalCardinalDirection = bPass ? ECardinalDirection::East : FinalCardinalDirection;
		bPass = UMathUtility::IsFloatInDualRange
		(
			LocalDirection, 
			CDConstraint.NorthConstraint, 
			CurrentCardinalDirection == ECardinalDirection::North
		);
		FinalCardinalDirection = bPass ? ECardinalDirection::North : FinalCardinalDirection;
		AnimationUtilData.CardinalDirection = FinalCardinalDirection;
	}
}

// Use a buffer to minimize flip floping of the direction
void UCharacterAnimationUtilityComponent::UpdateDirectionEnum(const float& DirectionMin, const float& DirectionMax, const float& Buffer)
{
	if (!IsCharacterMoving())
		return;
	bool bInRange = false;
	if (AnimationUtilData.MovementDirection == EMovementDirection::Forward)
	{
		bInRange = FMath::IsWithinInclusive(AnimationUtilData.Direction, DirectionMin - Buffer, DirectionMax + Buffer);
	}
	else
	{
		bInRange = FMath::IsWithinInclusive(AnimationUtilData.Direction, DirectionMin + Buffer, DirectionMax - Buffer);
	}
	if (bInRange)
	{
		AnimationUtilData.MovementDirection = EMovementDirection::Forward;
	}
	else
	{
		AnimationUtilData.MovementDirection = EMovementDirection::Backward;
	}
}

void UCharacterAnimationUtilityComponent::SetCurrentDirection()
{
	if(CharacterOwner)
	{
		const FVector CharacterVelocity = CharacterOwner->GetVelocity();
		FRotator DeltaRotator = AnimationUtilData.VelocityRotator - CharacterOwner->GetActorRotation();
		DeltaRotator.Normalize();
		AnimationUtilData.Direction = AnimationUtilData.bCharacterIsMoving ? DeltaRotator.Yaw : 0.0f;
	}
}

FAnimationUtilData UCharacterAnimationUtilityComponent::GetAnimationUtilData() const
{
	return AnimationUtilData;
}

ETargetingState UCharacterAnimationUtilityComponent::GetTargetingState() const
{
	return AnimationUtilData.TargetingState;
}

void UCharacterAnimationUtilityComponent::SetTargetingState(ETargetingState TargetingState)
{
	AnimationUtilData.TargetingState = TargetingState;
}
