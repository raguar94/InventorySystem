// Copyright Epic Games, Inc. All Rights Reserved.

#include "ISInventorySystem.h"

#if WITH_EDITOR
#include "ISettingsModule.h"
#include "ISettingsSection.h"
#include "Subsystems/ISInventoryGISSubsystems.h"
#endif // WITH_EDITOR

#define LOCTEXT_NAMESPACE "FISInventorySystemModule"

DEFINE_LOG_CATEGORY(LogInventorySystem);

void FISInventorySystemModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	RegisterSettings();
}

void FISInventorySystemModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UnregisterSettings();
}

void FISInventorySystemModule::RegisterSettings()
{
#if WITH_EDITOR
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		ISettingsSectionPtr SettingsSection = SettingsModule->RegisterSettings("Project", "Plugins", "InventorySystem",
			LOCTEXT("InventorySystemName", "Inventory System"),
			LOCTEXT("InventorySystemDescription", "Configure the GCInventorySystem plugin"),
			GetMutableDefault<UISInventoryGISSubsystems>()
		);
	}
#endif // WITH_EDITOR
}

void FISInventorySystemModule::UnregisterSettings()
{
#if WITH_EDITOR
	if (auto settingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		settingsModule->UnregisterSettings("Project", "Plugins", "InventorySystem");
	}
#endif
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FISInventorySystemModule, InventorySystem)