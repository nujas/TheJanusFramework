// Copyright 2018-2019 The Janus Project | 2034 Complex LLC. All Rights Reserved.

#include "AIStatCollectionActor.h"

// Sets default values
AAIStatCollectionActor::AAIStatCollectionActor()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
	IConsoleManager::Get().RegisterConsoleVariable( 
		TEXT("nujas.AI"),
		0,
		TEXT("Set what kind of Nujas AI debug window you would like to view.\n")
		TEXT("<=0: off\n")
		TEXT("  1: Aggression System\n")
		TEXT("  2: Positioning (not yet implemented)\n"),
		ECVF_Cheat
	);
}

// Called when the game starts or when spawned
void AAIStatCollectionActor::BeginPlay()
{
    Super::BeginPlay();
}

// Called every frame
void AAIStatCollectionActor::Tick(float DeltaTime)
{
    // listen for custom commands and react
	static const auto CVarNujasAI = IConsoleManager::Get().FindConsoleVariable(TEXT("nujas.AI"));
	if(CVarNujasAI)
	{
		NujasAIConsoleFlag = CVarNujasAI->GetInt();
	}
	
	if(NujasAIConsoleFlag == 1)
	{
		UE_LOG(LogTemp, Warning, TEXT("Console Flag works"));
	}
    Super::Tick(DeltaTime);
}