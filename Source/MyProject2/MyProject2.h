// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

DECLARE_LOG_CATEGORY_EXTERN(SwymlineLog, Log, All);

class MYPROJECT2_API FMyProject2Module : public FDefaultGameModuleImpl
{
	virtual void StartupModule() override;

	virtual void ShutdownModule() override;

	virtual bool SupportsDynamicReloading() override;

private:

	// Callback for when the settings were saved.
	bool HandleSettingsSaved();

	void RegisterSettings();

	void UnregisterSettings();
};