// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "EquipableDataAsset.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class MYPROJECT2_API UEquipableDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Equipment")
	FText EquipmentName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment")
	FName EquipmentSlot;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment")
	TSoftObjectPtr<USkeletalMesh> EquipmentMesh;
};
