// Copyright 2018-2019 The Janus Project | 2034 Complex LLC. All Rights Reserved.

#include "CharacterAnimationUtilityComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
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
	AnimationUtilData.bCharacterIsMoving = IsCharacterMoving();
	AnimationUtilData.bReceivingPlayerInput = IsThereMovementInput();
	SetCurrentSpeed();
	SetCurrentDirection();
	UpdateInAirData();
	UpdateCharacterLeanAcceleration();
	UpdateCharacterLeanRotation();
	SetCardinalEnum();
	SetDirectionEnum();
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

void UCharacterAnimationUtilityComponent::UpdateCharacterLeanRotation()
{
	float DeltaTime = FApp::GetDeltaTime();
	float Yaw = AnimationUtilData.Direction / DeltaTime;
	float Speed = AnimationUtilData.MovementSpeed;
	float LeanRotation = AnimationUtilData.LeanRotation;
	Yaw = FMath::Clamp(Yaw, -200.f, 200.f); // Magic
	Yaw = FMath::GetMappedRangeValueClamped(FVector2D(-200.f, 200.f), FVector2D(-.2f, .2f), Yaw); // Magic
	Speed = FMath::GetMappedRangeValueClamped(FVector2D(165.f, 375.f), FVector2D(0.f, 1.f), Speed); // Magic
	Yaw *= Speed;
	AnimationUtilData.LeanRotation = FMath::FInterpTo(LeanRotation, Yaw, DeltaTime, 8.f);
}

void UCharacterAnimationUtilityComponent::UpdateCharacterLeanAcceleration()
{
	// TODO: Probs not AccVector is supposed to be used here
	if (ChracterMovementComponent)
	{
		float MaxAcc;
		float NegativeBrakingDecWalking;
		float AccClamped;
		float DeltaTime = FApp::GetDeltaTime();
		FVector AccVector = ChracterMovementComponent->GetCurrentAcceleration();
		float AccSpeed = AccVector.Size2D();
		float MovementSpeed = AnimationUtilData.MovementSpeed;
		float LeanAcceleration = AnimationUtilData.LeanAcceleration;
		if(AccSpeed < 0.f)
		{
			NegativeBrakingDecWalking = -1 * ChracterMovementComponent->BrakingDecelerationWalking;
			AccClamped = FMath::GetMappedRangeValueClamped(FVector2D(0.f, NegativeBrakingDecWalking), FVector2D(0.f, -.2f), AccSpeed);
		}
		else
		{
			MaxAcc = ChracterMovementComponent->GetMaxAcceleration();
			AccClamped = FMath::GetMappedRangeValueClamped(FVector2D(0.f, MaxAcc), FVector2D(0.f, .2f), AccSpeed);
		}
		MovementSpeed = FMath::GetMappedRangeValueClamped(FVector2D(165.f, 375.f), FVector2D(0.f, 1.f), MovementSpeed); // Magic
		AccClamped *= MovementSpeed;
		AnimationUtilData.LeanAcceleration = FMath::FInterpTo(LeanAcceleration, AccClamped, DeltaTime, 8.f);
	}
}

void UCharacterAnimationUtilityComponent::SetCardinalEnum()
{
	if(IsCharacterMoving())
	{
		FCardinalDirectionConstraint CDConstraint = AnimationUtilData.CardinalDirectionConstraint;
		float LocalDirection = AnimationUtilData.Direction;
		ECardinalDirection CurrentCardinalDirection = AnimationUtilData.CardinalDirection;
		ECardinalDirection FinalCardinalDirection = ECardinalDirection::North;
		bool bPass = UMathUtility::IsFloatInDualRange(LocalDirection, CDConstraint.EastConstraint, LocalDirection > 0.0f);
		FinalCardinalDirection = bPass ? ECardinalDirection::South : ECardinalDirection::North;
		bPass = UMathUtility::IsFloatInDualRange(LocalDirection, CDConstraint.WestConstraint, CurrentCardinalDirection == ECardinalDirection::West);
		FinalCardinalDirection = bPass ? ECardinalDirection::West : FinalCardinalDirection;
		bPass = UMathUtility::IsFloatInDualRange(LocalDirection, CDConstraint.EastConstraint, CurrentCardinalDirection == ECardinalDirection::East);
		FinalCardinalDirection = bPass ? ECardinalDirection::East : FinalCardinalDirection;
		bPass = UMathUtility::IsFloatInDualRange(LocalDirection, CDConstraint.NorthConstraint, CurrentCardinalDirection == ECardinalDirection::North);
		FinalCardinalDirection = bPass ? ECardinalDirection::North : FinalCardinalDirection;
		AnimationUtilData.CardinalDirection = FinalCardinalDirection;
	}
}

void UCharacterAnimationUtilityComponent::SetDirectionEnum()
{
	if (IsCharacterMoving())
	{
		float LocalDirection = AnimationUtilData.Direction;
		FVector4 MovementConstraint = AnimationUtilData.MovementDirectionConstraint;
		EMovementDirection CurrentDirection = AnimationUtilData.MovementDirection;
		AnimationUtilData.MovementDirection = UMathUtility::IsFloatInDualRange(LocalDirection, 
			MovementConstraint, 
			CurrentDirection == EMovementDirection::Forward) 
			? EMovementDirection::Forward : EMovementDirection::Backward;
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
	if(CharacterOwner)
	{
		FVector CharacterVelocity = CharacterOwner->GetVelocity();
		AnimationUtilData.VelocityRotator = FVector(CharacterVelocity.X, CharacterVelocity.Y, 0.f).Rotation();
		FRotator DeltaRotator = AnimationUtilData.VelocityRotator - CharacterOwner->GetActorRotation();
		DeltaRotator.Normalize();
		FRotator NormalizedDeltaRotator = DeltaRotator;
		AnimationUtilData.Direction = AnimationUtilData.bCharacterIsMoving ? NormalizedDeltaRotator.Yaw : 0.0f;
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
