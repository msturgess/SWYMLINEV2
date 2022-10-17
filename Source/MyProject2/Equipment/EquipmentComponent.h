// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EquipmentComponent.generated.h"

class UEquipableDataAsset;
class UEquipmentSetDataAsset;

USTRUCT(BlueprintType)
struct FEquipmentSlot
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FGuid SlotId;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "EquipmentSlot")
	FName SlotName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EquipmentSlot")
	UEquipableDataAsset* EquippedAsset;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "EquipmentSlot")
	USkeletalMeshComponent* SkeletalMeshComp;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYPROJECT2_API UEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UEquipmentComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category="Equipment")
	void ApplyEquipmentSet(UEquipmentSetDataAsset* InEquipmentSet);

	//void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent);

public:
	/*UPROPERTY()
	TMap<FGuid, TSoftObjectPtr<USkeletalMeshComponent>> EquipMeshMap;*/

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FEquipmentSlot> EquipmentSlots;
};
