// Fill out your copyright notice in the Description page of Project Settings.

#include "MyProject2Editor.h"
#include "Modules/ModuleManager.h"

#define LOCTEXT_NAMESPACE "MyProject2EditorModule"


void FMyProject2EditorModule::StartupModule()
{
}

void FMyProject2EditorModule::ShutdownModule()
{

}

bool FMyProject2EditorModule::SupportsDynamicReloading()
{
	return true;
}

IMPLEMENT_PRIMARY_GAME_MODULE(FMyProject2EditorModule, MyProject2Editor, "MyProject2Editor");

#undef LOCTEXT_NAMESPACE