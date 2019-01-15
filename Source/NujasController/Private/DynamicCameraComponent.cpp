// Copyright 2018 The Janus Project | 2034 Complex LLC. All Rights Reserved.

#include "DynamicCameraComponent.h"

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

	// ...
}

// Called every frame
void UDynamicCameraComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}
