// Copyright 2018-2019 The Janus Project | 2034 Complex LLC. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Character.h"
#include "Components/ActorComponent.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/AnimInstance.h"

#include "Animation/AnimMontage.h"

#include "MontageUtilityComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class NUJASANIMATION_API UMontageUtilityComponent : public UActorComponent
{
	GENERATED_BODY()

	ACharacter *Owner;
	UAnimInstance *OwnerAnimInstance;
	UCharacterMovementComponent *OwnerMovement;

  public:
	// Sets default values for this component's properties
	UMontageUtilityComponent();

  protected:
	// Called when the game starts
	virtual void BeginPlay() override;

  public:

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

	UFUNCTION(BlueprintPure, Category = "Montage Utility")
	virtual UAnimMontage* GetMontageForAction(FName MontageName, int32 Index);

	UFUNCTION(BlueprintPure, Category = "Montage Utility")
	virtual int32 GetMontageActionLastIndex(FName MontageRowName);
};
