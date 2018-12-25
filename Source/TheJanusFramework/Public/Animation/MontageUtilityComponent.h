// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Character.h"
#include "Components/ActorComponent.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/AnimInstance.h"

#include "MontageUtilityComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class THEJANUSFRAMEWORK_API UMontageUtilityComponent : public UActorComponent
{
	GENERATED_BODY()

	ACharacter * owner;
	UAnimInstance * ownerAnimInstance;
	UCharacterMovementComponent * ownerMovement;

public:	
	// Sets default values for this component's properties
	UMontageUtilityComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Montage Utility")
		float GetCurrentMontageTime();

	UFUNCTION(BlueprintCallable, Category = "Montage Utility")
		float GetCurrentMontagePlayRate();

	UFUNCTION(BlueprintCallable, Category = "Montage Utility")
		void SetCurrentMontagePlayRate(float rate);

	UFUNCTION(BlueprintCallable, Category = "Montage Utility")
		void EnableRotationOverride();

	UFUNCTION(BlueprintCallable, Category = "Montage Utility")
		void DisableRotationOverride();

	
};
