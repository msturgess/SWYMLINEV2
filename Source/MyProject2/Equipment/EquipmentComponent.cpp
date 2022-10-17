// Fill out your copyright notice in the Description page of Project Settings.


#include "../Equipment/EquipmentComponent.h"
#include "../Equipment/EquipableDataAsset.h"
#include "../Equipment/EquipmentSetDataAsset.h"
#include "../Runtime/Engine/Classes/Engine/SkeletalMesh.h"

// Sets default values for this component's properties
UEquipmentComponent::UEquipmentComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UEquipmentComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UEquipmentComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UEquipmentComponent::ApplyEquipmentSet(UEquipmentSetDataAsset* InEquipmentSet)
{
	if (!InEquipmentSet)
		return;

	for (int i = 0; i < InEquipmentSet->Equipment.Num(); i++)
	{
		UEquipableDataAsset* curEquipable = InEquipmentSet->Equipment[i];

		for (int j = 0; j < EquipmentSlots.Num(); j++)
		{
			if(EquipmentSlots[j].SlotName != curEquipable->EquipmentSlot)
				continue;

			EquipmentSlots[j].EquippedAsset = curEquipable;
			EquipmentSlots[j].SkeletalMeshComp->SetSkeletalMesh(curEquipable->EquipmentMesh.LoadSynchronous());

			break;
		}
	}
}

//
//void UEquipmentComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
//{
//	if (!PropertyChangedEvent.Property)
//		return;
//
//	static const FName NAME_EquipSlots = FName(TEXT("EquipmentSlots"));
//
//	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UEquipmentComponent, EquipmentSlots))
//	{
//		if (PropertyChangedEvent.ChangeType == EPropertyChangeType::ArrayAdd)
//		{
//			for (int i = 0; i < EquipmentSlots.Num(); i++)
//			{
//				if(EquipmentSlots[i].SkeletalMeshComp)
//					continue;
//				
//				FTransform compTransform = FTransform::Identity;
//				EquipmentSlots[i].SlotId = FGuid::NewGuid();
//
//				const FName ComponentName = MakeUniqueObjectName(GetOuter(), USkeletalMeshComponent::StaticClass(), TEXT("Equipment_SkelMesh_"));
//				USkeletalMeshComponent* newSkeletalMeshComp = NewObject<USkeletalMeshComponent>(GetTransientPackage(), ComponentName, RF_ArchetypeObject);
//				TArray<UActorComponent*> addComponents{ newSkeletalMeshComp };
//
//				UBlueprint* ownerbp = Cast<UBlueprint>(GetOwner()->GetClass()->ClassGeneratedBy);
//
//				FKismetEditorUtilities::FAddComponentsToBlueprintParams addParams;
//				addParams.bKeepMobility = false;
//				addParams.HarvestMode = FKismetEditorUtilities::EAddComponentToBPHarvestMode::None;
//				
//				FKismetEditorUtilities::AddComponentsToBlueprint(ownerbp, addComponents, addParams);
//
//				EquipmentSlots[i].SkeletalMeshComp = newSkeletalMeshComp;
//
//				EquipMeshMap.Add(EquipmentSlots[i].SlotId, EquipmentSlots[i].SkeletalMeshComp);
//			}
//		}
//		else if (PropertyChangedEvent.ChangeType == EPropertyChangeType::ArrayRemove)
//		{
//			for (TPair<FGuid, TSoftObjectPtr<USkeletalMeshComponent>> meshPair : EquipMeshMap)
//			{
//				bool stillExists = false;
//
//				for (int i = 0; i < EquipmentSlots.Num(); i++)
//				{
//					if(meshPair.Key != EquipmentSlots[i].SlotId)
//						continue;
//
//					stillExists = true;
//					break;
//				}
//
//				if (stillExists)
//					continue;
//
//				GetOwner()->RemoveOwnedComponent(meshPair.Value.Get());
//				EquipMeshMap.Remove(meshPair.Key);
//			}
//		}
//		else if (PropertyChangedEvent.ChangeType == EPropertyChangeType::ArrayClear)
//		{
//			for (TPair<FGuid, TSoftObjectPtr<USkeletalMeshComponent>> meshPair : EquipMeshMap)
//			{
//				GetOwner()->RemoveOwnedComponent(meshPair.Value.Get());
//			}
//
//			EquipMeshMap.Reset();
//		}
//	}
//}