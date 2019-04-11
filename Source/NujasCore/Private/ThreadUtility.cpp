// Copyright 2018-2019 The Janus Project | 2034 Complex LLC. All Rights Reserved.

#include "ThreadUtility.h"

FManualAsyncTaskHandle UThreadUtility::CreateAsyncTask(const FNujasDelegate Delegate)
{
	FManualAsyncTaskHandle Handle;
	TSharedPtr<FAsyncTask<FNujasManualAsyncTask>> NewTask(new FAsyncTask<FNujasManualAsyncTask>());
	NewTask->GetTask().NujasTask = Delegate;
	Handle.AsyncTask = NewTask;
	Handle.StartAsyncTask();
	return Handle;
}

void UThreadUtility::InvalidateAsyncTask(UPARAM(ref) FManualAsyncTaskHandle& Handle)
{
	Handle.AsyncTask->EnsureCompletion();
	Handle.AsyncTask.Reset();
}
