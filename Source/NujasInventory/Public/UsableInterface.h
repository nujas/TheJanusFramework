// Copyright 2018-2019 The Janus Project | 2034 Complex LLC. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "UsableInterface.generated.h"

class AController;
class UCanvas;

// This class does not need to be modified.
UINTERFACE(MinimalAPI, Blueprintable)
class UUsableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class NUJASINVENTORY_API IUsableInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, DisplayName = "OnUsed")
	void ReceiveOnUsed(AController * User);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, DisplayName = "OnFocused")
	void ReceiveOnFocused(UCanvas * Canvas, AController * User);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, DisplayName = "OnUnfocused")
	void ReceiveOnUnfocused();
};
