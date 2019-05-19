// Copyright 2018-2019 The Janus Project | 2034 Complex LLC. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ViewportUtility.generated.h"

UCLASS()
class NUJASCORE_API UViewportUtility : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Janus Math Utility")
	// detect if the provided 2D location is within the viewport bounds
	static bool IsInViewport(FVector2D Position, APlayerController* PC);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Janus Math Utility")
	// project the actor's V3 into a V2 on screen positions
	static FVector2D GetActorOnScreenPosition(AActor* Target, APlayerController* PC, bool& bOutSuccess);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Janus Math Utility")
	// get the current size of the viewport
	static void GetViewportSize(APlayerController* PC, FVector2D& OutSize);
};