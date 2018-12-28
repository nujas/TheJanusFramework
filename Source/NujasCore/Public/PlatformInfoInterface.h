// Copyright 2018 The Janus Project | 2034 Complex LLC. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "PlatformUtility.h"

#include "PlatformInfoInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(Blueprintable)
class UPlatformInfoInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class IPlatformInfoInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Get Current Platform")
	ESupportedPlatform GetCurrentPlatform() const;
};
