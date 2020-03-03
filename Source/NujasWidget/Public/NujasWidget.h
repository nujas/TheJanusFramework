// Copyright 2018-2019 The Janus Project | 2034 Complex LLC. All Rights Reserved.

#pragma once

#include "UserWidget.h"
#include "NujasWidget.generated.h"

// We make the class abstract, as we don't want to create
// instances of this, instead we want to create instances.
UCLASS(Abstract)
class NUJASWIDGET_API UNujasWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UNujasWidget(const FObjectInitializer &ObjectInitializer);

    // Optionally override the Blueprint "Event Construct" event
    virtual void NativeConstruct() override;

    // Optionally override the tick event
    virtual void NativeTick(const FGeometry &MyGeometry, float InDeltaTime) override;
};