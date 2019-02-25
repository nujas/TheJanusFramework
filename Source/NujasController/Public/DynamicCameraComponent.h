// Copyright 2018-2019 The Janus Project | 2034 Complex LLC. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DynamicCameraComponent.generated.h"

class UCameraComponent;
class USpringArmComponent;

UENUM()
enum class EResetType : uint8
{
	LocationOffset =		1 << 0,
	RotationOffset =		1 << 1,
	AllOffset =				1 << 2
};

/*
* Asynchronous Camera Actor component.
* Utilize it for smooth camera rotations and tranformations at runtime.
* The most recent command will always stop previous commands in order to maintain responsiveness
*/
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class NUJASCONTROLLER_API UDynamicCameraComponent : public UActorComponent
{
	GENERATED_BODY()

	void ResetRotationOffsetTimer();
	void ResetLocationOffsetTimer();
	void Move();
	void Rotate();

	void ResetLocation(float CallbackFrequency = 0.1f, float OffsetSpeed = 1.0f);
	void ResetRotation(float CallbackFrequency = 0.1f, float OffsetSpeed = 1.0f);

	FTimerHandle RotationOffsetTimerHandle;
	FTimerHandle LocationOffsetTimerHandle;

	UCameraComponent* CameraComponent;
	USpringArmComponent* SpringArmComponent;
	AActor* Owner;

	FVector LocationOffset = FVector::ZeroVector;
	float LocationOffsetSpeed = 1.f;

	FRotator RotationOffset = FRotator::ZeroRotator;
	float RotationOffsetSpeed = 1.f;

	FVector CameraOriginLocation = FVector::ZeroVector;
	FRotator CameraOriginRotation = FRotator::ZeroRotator;

  public:
	// Sets default values for this component's properties
	UDynamicCameraComponent();

  protected:
	// Called when the game starts
	virtual void BeginPlay() override;

  public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
	
	// TODO: add ranges to these functions. Once found working values, switch speeds to a better tested default value
	// TODO: add functions: AddLocationOffset + AddRotationOffset. Utilize Move and Rotate for them
	UFUNCTION(BlueprintCallable, Category = "Dynamic Camera")
	void Rotate(FRotator RotationOffset, float CallbackFrequency = .01f, float Speed = 1.f);
	
	UFUNCTION(BlueprintCallable, Category = "Dynamic Camera")
	void Move(FVector LocationOffset, float CallbackFrequency = .01f, float Speed = 1.f);
	
	UFUNCTION(BlueprintCallable, Category = "Dynamic Camera")
	void Reset(EResetType ResetType = EResetType::AllOffset, float CallbackFrequency = .01f, float LocationOffsetSpeed = 1.f, float RotationOffsetSpeed = 1.f);
};
