// Copyright 2018-2019 The Janus Project | 2034 Complex LLC. All Rights Reserved.

#include "PlatformUtility.h"

#include "InputCoreTypes.h"
#include "Engine.h"

ESupportedPlatform UPlatformUtility::GetPlatformFromKey(const FKey &key) {
	if (key.IsGamepadKey())
	{
		return ESupportedPlatform::Playstation;
	}
	else {
		return ESupportedPlatform::Windows;
	}
}