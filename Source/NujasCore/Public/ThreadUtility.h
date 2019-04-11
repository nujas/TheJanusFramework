// Copyright 2018-2019 The Janus Project | 2034 Complex LLC. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Async/AsyncWork.h"
#include "Async/TaskGraphInterfaces.h"
#include "HAL/ThreadManager.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Blueprint/BlueprintSupport.h"
#include "ThreadUtility.generated.h"

DECLARE_DYNAMIC_DELEGATE(FNujasDelegate);

class FNujasManualAsyncTask : public FNonAbandonableTask
{
	friend class FAsyncTask<FNujasManualAsyncTask>;

public:

	FNujasManualAsyncTask()
	{
	};

	FNujasDelegate NujasTask;

protected:
	void DoWork() 
	{
		NujasTask.Execute();
	};
	FORCEINLINE TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(FMyTaskName, STATGROUP_ThreadPoolAsyncTasks);
	};
};

USTRUCT(BlueprintType)
struct FManualAsyncTaskHandle
{
	GENERATED_BODY()

	TSharedPtr<FAsyncTask<FNujasManualAsyncTask>> AsyncTask;

	void StartAsyncTask() const 
	{
		AsyncTask->StartBackgroundTask();
	};
};

/*
 * Threading function library to manage asynchronous calls away from the game thread 
 * When you create an async task it will run immediately
 * You must manually delete the async task before creating an exact duplicate task
 * It will throw an exception if you exit out of the game before invalidting an async task
 *
 * TODO: Add automatic AsyncTasks
 * TODO: The delegate is a multicast. At the moment you can only assign one task,
 *		 So there must be a feature to add additional tasks
 */
UCLASS()
class NUJASCORE_API UThreadUtility : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

  public:
	UFUNCTION(BlueprintCallable, Category = "Janus Thread Utility")
	static FManualAsyncTaskHandle CreateAsyncTask(const FNujasDelegate Delegate);
	UFUNCTION(BlueprintCallable, Category = "Janus Thread Utility")
	static void InvalidateAsyncTask(UPARAM(ref) FManualAsyncTaskHandle& Handle);
};