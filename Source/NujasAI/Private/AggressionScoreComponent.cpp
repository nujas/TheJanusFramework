// Copyright 2018-2019 The Janus Project | 2034 Complex LLC. All Rights Reserved.

#include "AggressionScoreComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "NujasAIGlobals.h"
#include "TimerManager.h"

UAggressionScoreComponent::UAggressionScoreComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UAggressionScoreComponent::BeginPlay()
{
    Super::BeginPlay();
	Owner = Cast<ACharacter>(this->GetOwner());
	InvalidateAggressionScoreTimer();
}

void UAggressionScoreComponent::BeginAggressionSystem(const TArray<AActor*> BotsToConsider)
{
	if(Owner) 
	{
		Bots = BotsToConsider;
		
		Owner->GetWorldTimerManager().SetTimer
		(
			AggressionScoreHandle,
			this, 
			&UAggressionScoreComponent::CalculateAggressionScores,
			1.f, 
			true
		);
	}
}

void UAggressionScoreComponent::StopAggressionSystem()
{
	InvalidateAggressionScoreTimer();
	HeapAggresiveActors.Empty();
	Bots.Empty();
}

void UAggressionScoreComponent::AddBot(AActor* const Bot)
{
	Bots.Add(Bot);
}

void UAggressionScoreComponent::CalculateAggressionScores()
{ 
	if (Bots.Num() > 0) 
	{
		HeapAggresiveActors.Empty();

		// do the aggression scoring here
		for (AActor* const Bot : Bots) 
		{
			/*
			* Removing a specific bot from anywhere in the list is expensive. 
			* Better to just check if still valid. 
			* The list doesn't grow larger than 20 items per battle.
			*/
			if (!Bot) continue;
			float Score = IAggressive::Execute_CanBecomeAggressive(Bot) ? 1.0 : 0.0;
			Score += IAggressive::Execute_GetAggressionPriority(Bot);

			if (IsPlayerCharacter) 
			{
				// is bot on screen? (Viewport utility)
				// angle from camera?
			}

			// distance from actor

			HeapAggresiveActors.Add(FAggressiveActorEntry(Bot, Score));
		
		}
		
		// heapify

		// Pop items and make them aggressive until run out of tokens

		// rest should be made calm
	}
}

inline void UAggressionScoreComponent::InvalidateAggressionScoreTimer()
{
	if (Owner)
	{
		Owner->GetWorldTimerManager().ClearTimer(AggressionScoreHandle);
	}
}

FAggressiveActorEntry::FAggressiveActorEntry()
{
	this->Score = 0;
	this->Bot = nullptr;
}

FAggressiveActorEntry::FAggressiveActorEntry(AActor* Bot, float Score)
{
	this->Bot = Bot;
	this->Score = Score;
}
