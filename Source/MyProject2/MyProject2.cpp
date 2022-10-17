// Fill out your copyright notice in the Description page of Project Settings.

#include "MyProject2.h"
#include "Modules/ModuleManager.h"
#if WITH_EDITOR
#include "ISettingsModule.h"
#include "ISettingsSection.h"
#include "ISettingsContainer.h"
#endif
#include "MyProject2/Savegame/SaveSubsystem.h"

#define LOCTEXT_NAMESPACE "MyProject2Module"

DEFINE_LOG_CATEGORY(SwymlineLog);

void FMyProject2Module::StartupModule()
{
	RegisterSettings();
}

void FMyProject2Module::ShutdownModule()
{
	if (UObjectInitialized())
	{
		UnregisterSettings();
	}
}

bool FMyProject2Module::SupportsDynamicReloading()
{
	return true;
}

// Callback for when the settings were saved.
bool FMyProject2Module::HandleSettingsSaved()
{
	USaveGameSettings* Settings = GetMutableDefault<USaveGameSettings>();
	bool ResaveSettings = false;

	// You can put any validation code in here and resave the settings in case an invalid
	// value has been entered

	if (ResaveSettings)
	{
		Settings->SaveConfig();
	}

	return true;
}

void FMyProject2Module::RegisterSettings()
{
	// Registering some settings is just a matter of exposing the default UObject of
	// your desired class, feel free to add here all those settings you want to expose
	// to your LDs or artists.
#if WITH_EDITOR

	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		// Create the new category
		ISettingsContainerPtr SettingsContainer = SettingsModule->GetContainer("Project");

		SettingsContainer->DescribeCategory("SaveSettings",
			LOCTEXT("RuntimeWDCategoryName", "SaveSettings"),
			LOCTEXT("RuntimeWDCategoryDescription", "Settings related to the save game system"));

		// Register the settings
		ISettingsSectionPtr SettingsSection = SettingsModule->RegisterSettings("Project", "SaveSettings", "General",
			LOCTEXT("RuntimeGeneralSettingsName", "General"),
			LOCTEXT("RuntimeGeneralSettingsDescription", "Base configuration for our game module"),
			GetMutableDefault<USaveGameSettings>()
		);

		// Register the save handler to your settings, you might want to use it to
		// validate those or just act to settings changes.
		if (SettingsSection.IsValid())
		{
			SettingsSection->OnModified().BindRaw(this, &FMyProject2Module::HandleSettingsSaved);
		}
	}
#endif
}

void FMyProject2Module::UnregisterSettings()
{
	// Ensure to unregister all of your registered settings here, hot-reload would
	// otherwise yield unexpected results.
#if WITH_EDITOR

	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->UnregisterSettings("Project", "SaveSettings", "General");
	}
#endif
}

IMPLEMENT_PRIMARY_GAME_MODULE(FMyProject2Module, MyProject2, "MyProject2");

#undef LOCTEXT_NAMESPACE