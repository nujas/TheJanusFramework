// Copyright 2018-2019 The Janus Project | 2034 Complex LLC. All Rights Reserved.

#include "BasicUsableObject.h"

// Sets default values
ABasicUsableObject::ABasicUsableObject()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void ABasicUsableObject::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ABasicUsableObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
