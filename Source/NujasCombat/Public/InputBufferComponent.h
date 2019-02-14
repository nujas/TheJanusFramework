#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlatformUtility.h"
#include "InputBufferComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FInputBufferDelegate);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FParameterizedInputBufferDelegate, ECommonInput, InputKey);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class NUJASCOMBAT_API UInputBufferComponent : public UActorComponent
{
	GENERATED_BODY()

	ECommonInput InputKey = ECommonInput::Default;
	bool bIsOpenBuffer = false; // TODO: Verify that the buffer shoul be closed by default

	void ConsumeInputBuffer();

  public:
	// Sets default values for this component's properties
	UInputBufferComponent();

	UPROPERTY(BlueprintAssignable, Category = "Input Buffer")
	FInputBufferDelegate OnInputBufferOpen;

	UPROPERTY(BlueprintAssignable, Category = "Input Buffer")
	FInputBufferDelegate OnInputBufferClose;

	UPROPERTY(BlueprintAssignable, Category = "Input Buffer")
	FParameterizedInputBufferDelegate OnInputBufferConsumed;

  protected:
	// Called when the game starts
	virtual void BeginPlay() override;

  public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

	void UpdateKey(ECommonInput Key);

	void OpenInputBuffer();

	void CloseInputBuffer();

	void ClearInputBuffer() { InputKey = ECommonInput::Default; };

	bool IsOpenBuffer() const { return bIsOpenBuffer; };

	ECommonInput GetStoredKey() const { return InputKey; };
};