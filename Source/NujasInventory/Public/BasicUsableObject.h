// Copyright 2018-2019 The Janus Project | 2034 Complex LLC. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "UsableInterface.h"

#include "BasicUsableObject.generated.h"

UCLASS()
class NUJASINVENTORY_API ABasicUsableObject : public AActor, public IUsableInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABasicUsableObject();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
