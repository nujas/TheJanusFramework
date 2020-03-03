// Copyright 2018-2019 The Janus Project | 2034 Complex LLC. All Rights Reserved.

#include "NujasWidgetModule.h"

#define LOCTEXT_NAMESPACE "FNujasWidgetModule"

void FNujasWidgetModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FNujasWidgetModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FNujasWidgetModule, NujasWidget)