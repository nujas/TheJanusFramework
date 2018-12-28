// Copyright 2018 The Janus Project | 2034 Complex LLC. All Rights Reserved.

#include "NujasCoreModule.h"

#define LOCTEXT_NAMESPACE "FNujasCoreModule"

void FNujasCoreModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FNujasCoreModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FNujasCoreModule, NujasCore)