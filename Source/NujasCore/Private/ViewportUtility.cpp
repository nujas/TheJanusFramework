// Copyright 2018-2019 The Janus Project | 2034 Complex LLC. All Rights Reserved.

#include "ViewportUtility.h"
#include "Kismet/GameplayStatics.h"

bool UViewportUtility::IsInViewport(FVector2D Position, APlayerController* PC)
{
	if(!PC) return false;
	FVector2D ScreenSize;
	GetViewportSize(PC, ScreenSize);
	return (Position.X > 0 && Position.Y > 0 && Position.X <= ScreenSize.X && Position.Y <= ScreenSize.Y);
}

FVector2D UViewportUtility::GetActorOnScreenPosition(AActor* Target, APlayerController* PC, bool& bOutSuccess)
{
	if(!PC) return FVector2D(NAN, NAN); 
	FVector2D ScreenPosition;
	bOutSuccess = UGameplayStatics::ProjectWorldToScreen(PC, Target->GetActorLocation(), ScreenPosition);
	return ScreenPosition;
}

void UViewportUtility::GetViewportSize(APlayerController* PC, FVector2D& OutSize)
{
	if(!PC) return;
	int32 SizeX = -1;
	int32 SizeY = -1;
	PC->GetViewportSize(SizeX, SizeY);
	OutSize.X = (float)SizeX;
	OutSize.Y = (float)SizeY;
}