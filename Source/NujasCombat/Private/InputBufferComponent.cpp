// Copyright 2018-2019 The Janus Project | 2034 Complex LLC. All Rights Reserved.
#include "InputBufferComponent.h"

UInputBufferComponent::UInputBufferComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UInputBufferComponent::BeginPlay()
{
    Super::BeginPlay();
}

// Called every frame
void UInputBufferComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // ...
}

void UInputBufferComponent::ConsumeInputBuffer()
{
	OnInputBufferConsumed.Broadcast(InputKey);
	ClearInputBuffer();
}

void UInputBufferComponent::UpdateKey(ECommonInput Key)
{
	InputKey = Key;
	if (IsOpenBuffer()) 
		ConsumeInputBuffer();
}

void UInputBufferComponent::OpenInputBuffer()
{
	bIsOpenBuffer = true;
	OnInputBufferOpen.Broadcast();
}

void UInputBufferComponent::CloseInputBuffer()
{
	bIsOpenBuffer = false;
	OnInputBufferClose.Broadcast();
	ConsumeInputBuffer();
}
