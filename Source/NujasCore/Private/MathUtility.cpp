// Copyright 2018-2019 The Janus Project | 2034 Complex LLC. All Rights Reserved.

#include "MathUtility.h"

bool UMathUtility::IsFloatInDualRange(float TargetFloat, FVector4 DualRange, bool bRangeSelect)
{
	float FinalMin;
	float FinalMax;

	if (bRangeSelect)
	{
		FinalMin = DualRange.X;
		FinalMax = DualRange.Y;
	}
	else
	{
		FinalMin = DualRange.Z;
		FinalMax = DualRange.W;
	}
	return (TargetFloat >= FinalMin && TargetFloat <= FinalMax);
}