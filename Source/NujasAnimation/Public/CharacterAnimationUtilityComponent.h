// Copyright 2018-2019 The Janus Project | 2034 Complex LLC. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CharacterAnimationUtilityComponent.generated.h"

class ACharacter;
class UCharacterMovementComponent;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class NUJASANIMATION_API UCharacterAnimationUtilityComponent : public UActorComponent
{
	GENERATED_BODY()

	ACharacter *Owner;
	UCharacterMovementComponent *OwnerMovement;

public:
	// Sets default values for this component's properties
	UCharacterAnimationUtilityComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Animation Utility")
	void UpdateCharacterRotationBasedOnMovement(float input);
};
