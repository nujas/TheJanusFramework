// Copyright 2018-2019 The Janus Project | 2034 Complex LLC. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Blueprint/BlueprintSupport.h"
#include "MathUtility.generated.h"

UCLASS()
class NUJASCORE_API UMathUtility : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Janus Math Utility")
	/**
	* Math function responsible for determining whether a float is within one of the 2 float ranges.
	* A float range is selected based on the bool
	*
	* @param TargetFloat			The float that will be tested
	* @param DualRange				The vector that contains the 2 ranges
	* @param RangeSelect			The bool that selects the range to check against (true: x(min) & y(max), false: z(min) & w(max))
	*/
	static bool IsFloatInDualRange(float TargetFloat, FVector4 DualRange, bool bRangeSelect);
};