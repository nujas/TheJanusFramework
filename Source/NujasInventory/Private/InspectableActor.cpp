// Copyright 2018-2019 The Janus Project | 2034 Complex LLC. All Rights Reserved.

#include "InspectableActor.h"
#include "Sound/DialogueWave.h"
#include "Text.h"

// Sets default values
AInspectableActor::AInspectableActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AInspectableActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AInspectableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

UDialogueWave * AInspectableActor::GetNextInfo()
{
	CurrentInfo = ActiveInfoList[NextInfoIndex];

	NextInfoIndex = (NextInfoIndex + 1) % ActiveInfoList.Num();

	return CurrentInfo;
}

UDialogueWave * AInspectableActor::GetPreviousInfo()
{
	CurrentInfo = ActiveInfoList[NextInfoIndex - 2];

	NextInfoIndex = (NextInfoIndex - 1) % ActiveInfoList.Num();

	return CurrentInfo;
}

