// Copyright 2018-2019 The Janus Project | 2034 Complex LLC. All Rights Reserved.

#include "DynamicCameraComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Core/Public/Misc/App.h"
#include "TimerManager.h"

// Sets default values for this component's properties
UDynamicCameraComponent::UDynamicCameraComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	/*
	*/

	// ...
}

// Called when the game starts
void UDynamicCameraComponent::BeginPlay()
{
	Super::BeginPlay();
	Owner = GetOwner();
	if(Owner)
	{
		CameraComponent = Cast<UCameraComponent>(Owner->GetComponentByClass(UCameraComponent::StaticClass()));
		SpringArmComponent = Cast<USpringArmComponent>(Owner->GetComponentByClass(USpringArmComponent::StaticClass()));
		if(SpringArmComponent && CameraComponent)
		{
			CameraOriginLocation = SpringArmComponent->SocketOffset;
			CameraOriginRotation = CameraComponent->RelativeRotation;
		}
	}
}

// Called every frame
void UDynamicCameraComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UDynamicCameraComponent::ResetRotationOffsetTimer()
{
	Owner->GetWorldTimerManager().ClearTimer(RotationOffsetTimerHandle);
}

void UDynamicCameraComponent::ResetLocationOffsetTimer()
{
	Owner->GetWorldTimerManager().ClearTimer(LocationOffsetTimerHandle);
}

void UDynamicCameraComponent::Move() // private
{
	if(SpringArmComponent)
	{
		FVector CurrentOffset = SpringArmComponent->SocketOffset;
		if(CurrentOffset.Equals(LocationOffset, .001f))
		{
			ResetLocationOffsetTimer();
			return;
		}
		FVector Target = FMath::VInterpTo(CurrentOffset, LocationOffset, FApp::GetDeltaTime(), LocationOffsetSpeed);
		SpringArmComponent->SocketOffset = Target;
	}
}

void UDynamicCameraComponent::Rotate() // private
{
	if(CameraComponent)
	{
		FRotator RelativeRotation = CameraComponent->RelativeRotation;
		if(RelativeRotation.Equals(RotationOffset, .001f))
		{
			ResetRotationOffsetTimer();
			return;
		}
		FRotator Target = FMath::RInterpTo(RelativeRotation, RotationOffset, FApp::GetDeltaTime(), RotationOffsetSpeed);
		CameraComponent->SetRelativeRotation(Target);
	}
}

void UDynamicCameraComponent::ResetLocation(float CallbackFrequency, float OffsetSpeed)
{
	ResetLocationOffsetTimer();
	LocationOffsetSpeed = OffsetSpeed;
	LocationOffset = CameraOriginLocation;
	Owner->GetWorldTimerManager().SetTimer(LocationOffsetTimerHandle, this, &UDynamicCameraComponent::Move, CallbackFrequency, true);
}

void UDynamicCameraComponent::ResetRotation(float CallbackFrequency, float OffsetSpeed)
{
	ResetRotationOffsetTimer();
	RotationOffsetSpeed = OffsetSpeed;
	RotationOffset = CameraOriginRotation;
	Owner->GetWorldTimerManager().SetTimer(RotationOffsetTimerHandle, this, &UDynamicCameraComponent::Rotate, CallbackFrequency, true);
}

void UDynamicCameraComponent::Rotate(FRotator Offset, float CallbackFrequency, float Speed)
{
	ResetRotationOffsetTimer();
	if(Owner)
	{
		RotationOffset = Offset;
		RotationOffsetSpeed = Speed;
		Owner->GetWorldTimerManager().SetTimer(RotationOffsetTimerHandle, this, &UDynamicCameraComponent::Rotate, CallbackFrequency, true);
	}
}

void UDynamicCameraComponent::Move(FVector Offset, float CallbackFrequency, float Speed)
{
	ResetLocationOffsetTimer();
	if(Owner)
	{
		LocationOffset = Offset;
		LocationOffsetSpeed = Speed;
		Owner->GetWorldTimerManager().SetTimer(LocationOffsetTimerHandle, this, &UDynamicCameraComponent::Move, CallbackFrequency, true);
	}
}

void UDynamicCameraComponent::Reset(EResetType ResetType, float CallbackFrequency, float LocationOffsetSpeed, float RotationOffsetSpeed)
{
	if (ResetType == EResetType::LocationOffset)
	{
		ResetLocation(CallbackFrequency, LocationOffsetSpeed);
	}
	else if(ResetType == EResetType::RotationOffset)
	{
		ResetRotation(CallbackFrequency, RotationOffsetSpeed);
	}
	else if(ResetType == EResetType::AllOffset)
	{
		ResetLocation(CallbackFrequency, LocationOffsetSpeed);
		ResetRotation(CallbackFrequency, RotationOffsetSpeed);
	}
}
