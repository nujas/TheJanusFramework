// Copyright 2018-2019 The Janus Project | 2034 Complex LLC. All Rights Reserved.

#pragma once

#include "Interface.h"
#include "NujasAIGlobals.generated.h"

UINTERFACE(Blueprintable)
class NUJASAI_API UAggressive : public UInterface
{
	GENERATED_BODY()
};

class IAggressive
{
	GENERATED_BODY()

public:
	// callback for when the actor turns aggressive
	UFUNCTION(BlueprintImplementableEvent, Category = "Aggressive")
	bool OnTurAgressive();

	// callback when the actor is deemed none-aggressive
	UFUNCTION(BlueprintImplementableEvent, Category = "Aggressive")
	bool OnTurnCalm();

	// helper func to determine the AI's priority. The priority is usually set manually depending on the AI
	UFUNCTION(BlueprintImplementableEvent, Category = "Aggressive")
	int32 GetAggressionPriority();

	// helper func to tell how many tokens does an AI take.
	UFUNCTION(BlueprintImplementableEvent, Category = "Aggressive")
	int32 GetTokenTax();

	// helper func to determine if the actor is in a state that would prevent them from becoming aggressive 
	UFUNCTION(BlueprintImplementableEvent, Category = "Aggressive")
	bool CanBecomeAggressive();
};