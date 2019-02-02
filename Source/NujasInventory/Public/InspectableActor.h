// Copyright 2018-2019 The Janus Project | 2034 Complex LLC. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BasicUsableObject.h"
#include "InspectableActor.generated.h"

class UDialogueWave;
class FText;

UCLASS()
class NUJASINVENTORY_API AInspectableActor : public ABasicUsableObject
{
	GENERATED_BODY()

	uint32 NextInfoIndex;
	UDialogueWave * CurrentInfo;

public:	
	// Sets default values for this actor's properties
	AInspectableActor();

	UPROPERTY(EditAnywhere, Category = "Inspectable")
	TArray<UDialogueWave *> ActiveInfoList;

	UPROPERTY(EditAnywhere, Category = "Inspectable")
	FText Title;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inspectable")
	UDialogueWave * GetNextInfo();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inspectable")
	UDialogueWave * GetPreviousInfo();

};
