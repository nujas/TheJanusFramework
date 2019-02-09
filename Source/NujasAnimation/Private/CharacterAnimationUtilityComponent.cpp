// Copyright 2018-2019 The Janus Project | 2034 Complex LLC. All Rights Reserved.

#include "CharacterAnimationUtilityComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Core/Public/Misc/App.h"
#include "Engine/Classes/Kismet/KismetMathLibrary.h"

UCharacterAnimationUtilityComponent::UCharacterAnimationUtilityComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	AnimationUtilData.MovementDirectionConstraints = FVector4(-100, 100, -80, 80); // Default
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
	AnimationUtilData.bCharacterIsMoving = IsCharacterMoving();
	AnimationUtilData.bReceivingPlayerInput = IsThereMovementInput();
	SetCurrentSpeed();
	SetCurrentDirection();
	UpdateInAirData();
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

void UCharacterAnimationUtilityComponent::SetCardinalEnum()
{
	if(IsCharacterMoving())
	{
		// TODO: Add the magic numbers to the AnimationUtilData and make them openly editable
		float LocalDirection = AnimationUtilData.Direction;
		ECardinalDirection CurrentCardinalDirection = AnimationUtilData.CardinalDirection;
		ECardinalDirection FinalCardinalDirection = ECardinalDirection::North;
		bool Pass = IsFloatInDualRange(LocalDirection, FVector4(130.f, 180.f, -180.f, -130.f), LocalDirection > 0.0f);
		FinalCardinalDirection = Pass ? ECardinalDirection::South : ECardinalDirection::North;
		Pass = IsFloatInDualRange(LocalDirection, FVector4(-140.f, -40.f, -130.f, 50.f), CurrentCardinalDirection == ECardinalDirection::West);
		FinalCardinalDirection = Pass ? ECardinalDirection::West : FinalCardinalDirection;
		Pass = IsFloatInDualRange(LocalDirection, FVector4(40.f, 140.f, 50.f, 130.f), CurrentCardinalDirection == ECardinalDirection::East);
		FinalCardinalDirection = Pass ? ECardinalDirection::East : FinalCardinalDirection;
		Pass = IsFloatInDualRange(LocalDirection, FVector4(-50.f, 50.f, -40.f, 40.f), CurrentCardinalDirection == ECardinalDirection::North);
		FinalCardinalDirection = Pass ? ECardinalDirection::North : FinalCardinalDirection;
		AnimationUtilData.CardinalDirection = FinalCardinalDirection;
	}
}

void UCharacterAnimationUtilityComponent::SetDirectionEnum()
{
	if (IsCharacterMoving())
	{
		float LocalDirection = AnimationUtilData.Direction;
		FVector4 MovementConstraints = AnimationUtilData.MovementDirectionConstraints;
		EMovementDirection CurrentDirection = AnimationUtilData.MovementDirection;
		AnimationUtilData.MovementDirection = IsFloatInDualRange(LocalDirection, 
			MovementConstraints, 
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
		AnimationUtilData.VelocityRotator = UKismetMathLibrary::Conv_VectorToRotator(FVector(CharacterVelocity.X, CharacterVelocity.Y, 0.f));
		FRotator NormalizedDeltaRotator = UKismetMathLibrary::NormalizedDeltaRotator(AnimationUtilData.VelocityRotator, CharacterOwner->GetActorRotation());
		AnimationUtilData.Direction = AnimationUtilData.bCharacterIsMoving ? NormalizedDeltaRotator.Yaw : 0.0f;
	}
}

FAnimationUtilData UCharacterAnimationUtilityComponent::GetAnimationUtilData() const
{
	return AnimationUtilData;
}

bool UCharacterAnimationUtilityComponent::IsFloatInDualRange(float TargetFloat, FVector4 DualRange, bool RangeSelect)
{
	float FinalMin;
	float FinalMax;

	if (RangeSelect)
	{
		FinalMin = DualRange.X;
		FinalMax = DualRange.Y;
	}
	else
	{
		FinalMin = DualRange.Z;
		FinalMax = DualRange.W;
	}
	return UKismetMathLibrary::InRange_FloatFloat(TargetFloat, FinalMin, FinalMax);
}
