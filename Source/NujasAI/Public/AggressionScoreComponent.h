#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AggressionScoreComponent.generated.h"


class ACharacter;

USTRUCT()
struct FAggressiveActorEntry
{
	GENERATED_BODY()

	FAggressiveActorEntry();
	FAggressiveActorEntry(AActor* Bot, float Score);

	AActor* Bot;
	float Score;
};

static const uint32 DEFAULT_AGGRESSION_TOKENS = 14;

UCLASS(ClassGroup = (NujasAI), meta = (BlueprintSpawnableComponent), HideCategories=(Tags, Cooking, Collision, ComponentReplication, Activation, AssetUserData))
class NUJASAI_API UAggressionScoreComponent : public UActorComponent
{
	GENERATED_BODY()

		// each enemy with a specific interface should be evaluated periodically (on a timer) by this component
		// 
		// each AI has the opportunity to become aggressive based on parameters listed below
		// by default, every AI must not be aggressive
		// 
		// each timer iteration all enemies with the Aggressive Interface must be collected, regardless of whether they are aggressive or not
		// every collected actor will be re-evaluated on whether they should remain aggressive or not
		// 
		// every ai has an aggression score count and the count will be evaluated based on there paramters:
		//		- Primary		-> Can I become aggressive? (not in hit reaction or in a state blocking them indefinetely)
		//		- Secondary		-> Aggression Rank based on distance to target
		//		- Misc			-> Action Rank: a) "on/off screen" b) angle "from camera" (needs to be toggled) c) distance from target
		//
		// once calculated, send it to a max heap of structs that stores the aggression score and the object
		// once every enemy is in the heap, pop the actors and force them into aggression until run out of available tokens
		//
		// every other actor should be immediately set to calm

		// todo: add player current target later when start revamping the targeting mechanic

	void CalculateAggressionScores();

	inline void InvalidateAggressionScoreTimer();

	UPROPERTY()
	// heapify array to sort bots based on their aggression score 
	TArray<FAggressiveActorEntry> HeapAggresiveActors;

	UPROPERTY()
	// the bots that will be considered for calculating the aggression score
	TArray<AActor*> Bots;
	
	UPROPERTY()
	FTimerHandle AggressionScoreHandle;

	UPROPERTY()
	ACharacter* Owner;

public:
    // Sets default values for this component's properties
    UAggressionScoreComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aggression Params", meta = (ClampMin = "0", ClampMax = "100"))
	// How many tokens can the aggression system give out for Bots to become aggressive 
	int32 MaxAggressionTokens = DEFAULT_AGGRESSION_TOKENS;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aggression Params")
	// Is this component attached to the player character
	bool IsPlayerCharacter = true;

protected:
    // Called when the game starts
    virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable, Category = "Aggression")
	void BeginAggressionSystem(const TArray<AActor*> BotsToConsider);

	UFUNCTION(BlueprintCallable, Category = "Aggression")
	void StopAggressionSystem();

	UFUNCTION(BlueprintCallable, Category = "Aggression")
	/*
	* The system can only add bots. 
	* When the encounter is over, the system will clean out leftover garbage.
	*/
	void AddBot(AActor* const Bot);
};