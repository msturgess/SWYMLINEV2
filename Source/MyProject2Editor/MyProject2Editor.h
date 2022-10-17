// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

class MYPROJECT2EDITOR_API FMyProject2EditorModule : public FDefaultGameModuleImpl
{
	virtual void StartupModule() override;

	virtual void ShutdownModule() override;

	virtual bool SupportsDynamicReloading() override;
};