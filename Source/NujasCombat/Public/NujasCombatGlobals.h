// Copyright 2018-2019 The Janus Project | 2034 Complex LLC. All Rights Reserved.

#pragma once

#include "Interface.h"
#include "NujasCombatGlobals.generated.h"

USTRUCT(Blueprintable)
struct FImpactType
{
	GENERATED_BODY()

	bool bCritical;

	float DamageAmount;
	
	float LaunchStrength;
	
	FVector HitLocation;
	
	FRotator HitNormalRotation;
	
	AActor* DamageDealer;
};

UINTERFACE(Blueprintable)
class NUJASCOMBAT_API UTargetable : public UInterface
{
	GENERATED_BODY()
};

UINTERFACE(Blueprintable)
class NUJASCOMBAT_API UAttackable : public UInterface
{
	GENERATED_BODY()
};

UINTERFACE(Blueprintable)
class NUJASCOMBAT_API UEnemy : public UInterface
{
	GENERATED_BODY()
};

class ITargetable
{
	GENERATED_BODY()
public:
	/* Call on Actor that got selected by the targeting component */
	UFUNCTION(BlueprintImplementableEvent, Category = "Dynamic Targeting")
	bool OnSelected();

	/* Call on Actor that got deselected by the targeting component */
	UFUNCTION(BlueprintImplementableEvent, Category = "Dynamic Targeting")
	bool OnDeselected();

	/* Call on Actor if you want to make sure that the actor is targetable */
	UFUNCTION(BlueprintImplementableEvent, Category = "Dynamic Targeting")
	bool IsTargetable();
};

class IAttackable
{
	GENERATED_BODY()
public:
	/* Check if the enemy has been stunned */
	UFUNCTION(BlueprintImplementableEvent, Category = "Attackable")
	bool HandleHit(FImpactType Impact);

	/* Detect if the enemy has entered attacking phase */
	UFUNCTION(BlueprintImplementableEvent, Category = "Attackable")
	bool IsImpact();

	/* Detect if the enemy is currently launching a projectile */
	UFUNCTION(BlueprintImplementableEvent, Category = "Attackable")
	bool IsAlive();
};

class IEnemy
{
	GENERATED_BODY()
public:
	/* Check if the enemy has been stunned */
	UFUNCTION(BlueprintImplementableEvent, Category = "Enemy")
	bool IsStunned();

	/* Detect if the enemy has entered attacking phase */
	UFUNCTION(BlueprintImplementableEvent, Category = "Enemy")
	bool IsAttacking();

	/* Detect if the enemy is currently launching a projectile */
	UFUNCTION(BlueprintImplementableEvent, Category = "Enemy")
	bool IsShooting();
};