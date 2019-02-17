#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlatformUtility.h"
#include "InputBufferComponent.generated.h"

UENUM(BlueprintType)
enum class EInputKeyType : uint8
{
	Default			UMETA(DisplayName = "Default"),
	LightAttack		UMETA(DisplayName = "Light Attack"),
	HeavyAttack		UMETA(DisplayName = "Heavy Attack"),
	ThrustAttack	UMETA(DisplayName = "Thrust Attack"),
	SpecialAttack	UMETA(DisplayName = "Special Attack"),
	AirAttack		UMETA(DisplayName = "Air Attack"),
	ParryAttack		UMETA(DisplayName = "Parry Attack")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FInputBufferDelegate);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FParameterizedInputBufferDelegate, EInputKeyType, InputKey);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class NUJASCOMBAT_API UInputBufferComponent : public UActorComponent
{
	GENERATED_BODY()

	EInputKeyType InputKey = EInputKeyType::Default;
	bool bIsOpenBuffer = false;

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
	
	UFUNCTION(BlueprintCallable, Category = "Input Buffer")
	void UpdateKey(EInputKeyType Key);
	
	UFUNCTION(BlueprintCallable, Category = "Input Buffer")
	void OpenInputBuffer();
	
	UFUNCTION(BlueprintCallable, Category = "Input Buffer")
	void CloseInputBuffer();

	UFUNCTION(BlueprintCallable, Category = "Input Buffer")
	void ClearInputBuffer() { InputKey = EInputKeyType::Default; };

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Input Buffer")
	bool IsOpenBuffer() const { return bIsOpenBuffer; };

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Input Buffer")
	EInputKeyType GetStoredKey() const { return InputKey; };
};