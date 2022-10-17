// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "EquipmentSetDataAsset.generated.h"

class UEquipableDataAsset;

/**
 * 
 */
UCLASS(BlueprintType)
class MYPROJECT2_API UEquipmentSetDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Equipment")
	TArray< UEquipableDataAsset* > Equipment;
};
