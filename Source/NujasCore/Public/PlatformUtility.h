// Copyright 2018-2019 The Janus Project | 2034 Complex LLC. All Rights Reserved.

#pragma once


#include "Kismet/BlueprintFunctionLibrary.h"

#include "CoreMinimal.h"

#include "Engine/Texture2D.h"

#include "Engine/DataTable.h"
#include "Blueprint/BlueprintSupport.h"

#include "PlatformUtility.generated.h"

UENUM(BlueprintType)        //"BlueprintType" is essential specifier
enum class ESupportedPlatform : uint8
{
	Default     UMETA(DisplayName = "Default"),
	Windows     UMETA(DisplayName = "Windows"),
	Playstation         UMETA(DisplayName = "Playstation"),
	XBox         UMETA(DisplayName = "XBox"),
};

UENUM(BlueprintType)        //"BlueprintType" is essential specifier
enum class ECommonInput : uint8
{
	Default     UMETA(DisplayName = "Default"),
	Interact      UMETA(DisplayName = "Interact"),
	Sprint         UMETA(DisplayName = "Sprint"),
	Finisher         UMETA(DisplayName = "Finisher"),
	Move          UMETA(DisplayName = "Move"),
	Rotate        UMETA(DisplayName = "Rotate"),
	Dash          UMETA(DisplayName = "Dash"),
	Aim           UMETA(DisplayName = "Aim"),
	Shield           UMETA(DisplayName = "Shield"),
	LightAttack          UMETA(DisplayName = "LightAttack"),
	HeavyAttack          UMETA(DisplayName = "HeavyAttack"),
	QuickTurn          UMETA(DisplayName = "QuickTurn"),
	Pause          UMETA(DisplayName = "Pause"),
	Restart          UMETA(DisplayName = "Restart"),
};

USTRUCT(BlueprintType)
struct FPlatformInputTexture : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config")
	TMap<ESupportedPlatform, UTexture2D*> PlatformTextureMap;
};

UCLASS()
class NUJASCORE_API UPlatformUtility : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Platform Utility")
	static ESupportedPlatform GetPlatformFromKey(const struct FKey &key);
};