// Fill out your copyright notice in the Description page of Project Settings.


#include "MyProject2/Savegame/SaveSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "MyProject2/MyProject2.h"

FString USaveData::ConvertStringToHash(FString InString)
{
	return FMD5::HashAnsiString(*InString);
}

void USaveGameInstance::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);

	for (int i = 0; i < SaveData.Num(); i++)
	{
		USaveData* curSaveData = SaveData[i];

		UClass* objClass = nullptr;

		if (Ar.IsSaving())
		{
			if (curSaveData == nullptr)
				continue;

			objClass = curSaveData->GetClass();

			Ar << objClass;
			objClass->SerializeBin(Ar, curSaveData);
		}
		else
		{
			Ar << objClass;

			if (objClass == nullptr)
			{
				UE_LOG(SwymlineLog, Log, TEXT("Missing class when loading save file this can be cause by classes getting renamed or destroyed, generate a new save id"));
				continue;
			}

			curSaveData = NewObject<USaveData>(this, objClass);
			objClass->SerializeBin(Ar, curSaveData);
			SaveData[i] = curSaveData;
		}
	}
}

void USaveGameInstance::OnSave()
{
	for (int i = 0; i < SaveData.Num(); i++)
	{
		USaveData* curSaveData = SaveData[i];

		if(curSaveData == nullptr)
			continue;

		curSaveData->OnSave();
	}
}

void USaveGameInstance::OnLoad()
{
	for (int i = 0; i < SaveData.Num(); i++)
	{
		USaveData* curSaveData = SaveData[i];

		if (curSaveData == nullptr)
			continue;

		curSaveData->OnLoad();
	}
}

void USaveSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	m_userSave = nullptr;
	m_gameSave = nullptr;

	USaveGameSettings* saveSettings = GetMutableDefault<USaveGameSettings>();

	m_userSaveName = saveSettings->UserSaveName;

	GetGameSaveInstance();
}

void USaveSubsystem::RequestSaveUserInstance()
{
	USaveGameSettings* saveSettings = GetMutableDefault<USaveGameSettings>();

	USaveGameInstance* saveInstance = GetUserSaveInstance();

	GetGameInstance()->GetTimerManager().SetTimer(m_userSaveTimerHndl, FTimerDelegate::CreateUObject(this, &USaveSubsystem::SaveGameAsync, saveInstance, m_userSaveName), saveSettings->SaveTime, false, saveSettings->SaveTime);
}

void USaveSubsystem::RequestSaveGameInstance()
{
	USaveGameSettings* saveSettings = GetMutableDefault<USaveGameSettings>();

	USaveGameInstance* saveInstance = GetGameSaveInstance();

	GetGameInstance()->GetTimerManager().SetTimer(m_gameSaveTimerHndl, FTimerDelegate::CreateUObject(this, &USaveSubsystem::SaveGameAsync, saveInstance, saveSettings->GameSaveName), saveSettings->SaveTime, false, saveSettings->SaveTime);
}

void USaveSubsystem::ForceSaveUserInstance()
{
	USaveGameInstance* saveInstance = GetUserSaveInstance();

	SaveGameImmediate(saveInstance, m_userSaveName);
}

void USaveSubsystem::ForceSaveGameInstance()
{
	USaveGameSettings* saveSettings = GetMutableDefault<USaveGameSettings>();

	USaveGameInstance* saveInstance = GetGameSaveInstance();

	SaveGameImmediate(saveInstance, saveSettings->GameSaveName);
}

void USaveSubsystem::LoadUserSaveInstance(FString InUserId)
{
	USaveGameSettings* saveSettings = GetMutableDefault<USaveGameSettings>();

	FString newSaveName = InUserId + "_" + saveSettings->UserSaveName;

	if (newSaveName == m_userSaveName)
		return;

	if (m_userSave != nullptr)
	{
		SaveGameImmediate(m_userSave, m_userSaveName);
	}

	m_userSave = nullptr;
	m_userSaveName = newSaveName;
	
	GetUserSaveInstance();
}

USaveData* USaveSubsystem::GetUserSaveData(TSubclassOf<USaveData> SaveDataType)
{
	USaveGameInstance* saveInstance = GetUserSaveInstance();

	return GetSaveData(saveInstance, SaveDataType);
}

USaveData* USaveSubsystem::GetGameSaveData(TSubclassOf<USaveData> SaveDataType)
{
	USaveGameInstance* saveInstance = GetGameSaveInstance();

	return GetSaveData(saveInstance, SaveDataType);
}

FString USaveSubsystem::ConvertStringToHash(FString InString)
{
	return FMD5::HashAnsiString(*InString);
}

USaveData* USaveSubsystem::GetSaveData(USaveGameInstance* SaveInstance, TSubclassOf<USaveData> SaveDataType)
{
	if (SaveInstance == nullptr)
		return nullptr;

	return SaveInstance->GetOrCreateSaveData(SaveDataType.Get());
}

USaveGameInstance* USaveSubsystem::GetUserSaveInstance()
{
	if (m_userSave != nullptr)
		return m_userSave;

	USaveGameSettings* saveSettings = GetMutableDefault<USaveGameSettings>();

	m_userSave = LoadSaveInstance(m_userSaveName);

	if (m_userSave != nullptr && m_userSave->SaveId != saveSettings->SaveId)
	{
		m_userSave = nullptr;
	}

	if (m_userSave != nullptr)
	{
		m_userSave->OnLoad();

		return m_userSave;
	}

	m_userSave = CreateNewSaveInstance();

	m_userSave->SaveId = saveSettings->SaveId;

	return m_userSave;
}

USaveGameInstance* USaveSubsystem::GetGameSaveInstance()
{
	if (m_gameSave != nullptr)
		return m_gameSave;

	USaveGameSettings* saveSettings = GetMutableDefault<USaveGameSettings>();

	m_gameSave = LoadSaveInstance(saveSettings->GameSaveName);

	if (m_gameSave != nullptr && m_gameSave->SaveId != saveSettings->SaveId)
	{
		m_gameSave = nullptr;
	}

	if (m_gameSave != nullptr)
	{
		m_gameSave->OnLoad();

		return m_gameSave;
	}

	m_gameSave = CreateNewSaveInstance();

	m_gameSave->SaveId = saveSettings->SaveId;

	return m_gameSave;
}

USaveGameInstance* USaveSubsystem::CreateNewSaveInstance()
{
	USaveGameInstance* newSaveInstance = Cast<USaveGameInstance>(UGameplayStatics::CreateSaveGameObject(USaveGameInstance::StaticClass()));

	return newSaveInstance;
}

USaveGameInstance* USaveSubsystem::LoadSaveInstance(FString SaveName)
{
	USaveGameInstance* saveInstance = Cast<USaveGameInstance>(UGameplayStatics::LoadGameFromSlot(SaveName, 0));

	return saveInstance;
}

void USaveSubsystem::SaveGameAsync(USaveGameInstance* SaveInstance, FString SaveName)
{
	if (SaveInstance == nullptr)
		return;

	SaveInstance->OnSave();

	UGameplayStatics::AsyncSaveGameToSlot(SaveInstance, SaveName, 0);
}

void USaveSubsystem::SaveGameImmediate(USaveGameInstance* SaveInstance, FString SaveName)
{
	if (SaveInstance == nullptr)
		return;

	SaveInstance->OnSave();

	UGameplayStatics::SaveGameToSlot(SaveInstance, SaveName, 0);
}