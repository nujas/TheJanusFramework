#include "NujasWidget.h"

UNujasWidget::UNujasWidget(const FObjectInitializer &ObjectInitializer)
    : Super(ObjectInitializer)
{
	
}

void UNujasWidget::NativeConstruct()
{
    // Do some custom setup

    // Call the Blueprint "Event Construct" node
    Super::NativeConstruct();
}

void UNujasWidget::NativeTick(const FGeometry &Geometry, float InDeltaTime)
{
    // Make sure to call the base class's NativeTick function
    Super::NativeTick(Geometry, InDeltaTime);

    // Do your custom tick stuff here
}