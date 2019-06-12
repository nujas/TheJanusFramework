// Copyright 2018-2019 The Janus Project | 2034 Complex LLC. All Rights Reserved.
#include "TargetSensingComponent.h"

bool UTargetSensingComponent::IsActorWithinScreenRange(AActor* Enemy, float& DotProduct)
{
	return false;
}

bool UTargetSensingComponent::IsWithinSensingRange(float SquaredDistanceToEnemy)
{
	return false;
}

UTargetSensingComponent::UTargetSensingComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UTargetSensingComponent::BeginPlay()
{
	if(AActor* Owner = GetOwner())
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
}

void UTargetSensingComponent::RemoveEnemy(AActor* Enemy)
{
}

AActor * UTargetSensingComponent::SearchForClosestEnemy()
{
	return nullptr;
}

AActor * UTargetSensingComponent::GetClosestEnemy()
{
	return nullptr;
}

int32 UTargetSensingComponent::EnemyCountInBounds()
{
	return int32();
}
