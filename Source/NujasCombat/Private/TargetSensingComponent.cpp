// Copyright 2018-2019 The Janus Project | 2034 Complex LLC. All Rights Reserved.
#include "TargetSensingComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "NujasCombatGlobals.h"

AActorSensor::AActorSensor()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AActorSensor::BeginPlay()
{
}


FSensingViewSpaceResponse UTargetSensingComponent::IsEnemyInScreenView(const AActor* Enemy)
{
	const FVector OwnerLocation = Owner->GetActorLocation();
	const FVector EnemyLocation = Enemy->GetActorLocation();

	FVector Direction = EnemyLocation - OwnerLocation;
	Direction.Z = 0.f;
	Direction.Normalize();

	FVector CameraForward = PlayerCamera->GetForwardVector();
	CameraForward.Z = 0.f;
	CameraForward.Normalize();

	const float Dot = FVector::DotProduct(CameraForward, Direction);
	const bool IsInRange = (Dot <= ScreenRange) ? true : false;
	return {IsInRange, Dot};
}

FSensingDistanceResponse UTargetSensingComponent::IsEnemyInSensingDistance(const AActor* Enemy)
{
	const float DistSqrd = FVector::DistSquared(Owner->GetActorLocation(), Enemy->GetActorLocation());
	const bool isInRange = (DistSqrd < SquaredDistanceSensingRange) ? true : false;
	return {isInRange, DistSqrd};
}

UTargetSensingComponent::UTargetSensingComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UTargetSensingComponent::BeginPlay()
{
	Owner = GetOwner();
	if(Owner)
	{
		if(PlayerCamera = MakeShareable(Owner->FindComponentByClass<UCameraComponent>()))
		{
			SetComponentTickEnabled(true);
		}
	}

	Super::BeginPlay();
}

// Called every frame
void UTargetSensingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	// ...
}

void UTargetSensingComponent::AddEnemy(AActor* Enemy)
{
	if(!Cast<IEnemy>(Enemy) || SensorMap.Contains(Enemy)) return;
	if(UWorld* const World = this->GetWorld())
	{
		AActorSensor* NewSensor = World->SpawnActor<AActorSensor>();
		SensorMap.Add(Enemy, NewSensor);
		OnEnemyEnter.Broadcast();
	}
}

void UTargetSensingComponent::RemoveEnemy(AActor* Enemy)
{
	if(!SensorMap.Contains(Enemy)) return;

	SensorMap[Enemy]->Destroy();
	SensorMap.Remove(Enemy);
	OnEnemyExit.Broadcast();
}

AActor* UTargetSensingComponent::SearchForClosestEnemy()
{
	ClosestEnemy = nullptr;
	for (const auto& EnemyPair : SensorMap)
	{
		AActor* const Enemy = EnemyPair.Key;
		if(!IAttackable::Execute_IsAlive(Enemy))
			continue;
		const FSensingDistanceResponse SD = IsEnemyInSensingDistance(Enemy);
		if(!SD.bIsInRange)
			continue;
		const FVector OwnerLocation = Owner->GetActorLocation();
		const FVector EnemyLocation = Enemy->GetActorLocation();
		const float CurrentDistance = FVector::DistSquared(OwnerLocation, EnemyLocation);
		const FSensingViewSpaceResponse VS = IsEnemyInScreenView(Enemy);
		if((DistSqrdToClosestEnemy == NAN || CurrentDistance < DistSqrdToClosestEnemy) && VS.bIsInView)
		{
			DistSqrdToClosestEnemy = CurrentDistance;
			DotProdToClosestEnemy = VS.DotProductFromViewToTarget;
			ClosestEnemy = Enemy;
		}
	}
	return ClosestEnemy;
}

AActor* UTargetSensingComponent::GetClosestEnemy()
{
	return ClosestEnemy;
}

int32 UTargetSensingComponent::GetEnemyNumInBounds()
{
	return SensorMap.Num();
}