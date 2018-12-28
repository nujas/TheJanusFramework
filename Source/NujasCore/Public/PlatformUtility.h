// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

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
};

USTRUCT(BlueprintType)
struct FPlatformInputTexture : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config")
	TMap<ESupportedPlatform, UTexture2D*> PlatformTextureMap;
};