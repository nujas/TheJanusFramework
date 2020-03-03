// Copyright 2018-2019 The Janus Project | 2034 Complex LLC. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "AIStatCollectionActor.generated.h"

UCLASS()
class NUJASDEBUG_API AAIStatCollectionActor : public AActor
{
    GENERATED_BODY()

    // get the value on the game thread
    int32 NujasAIConsoleFlag = 0;

public:
    // Sets default values for this actor's properties
    AAIStatCollectionActor();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;
};