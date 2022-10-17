// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameFramework/SaveGame.h"
#include "SaveSubsystem.generated.h"

UCLASS(Config=Save, defaultconfig, meta=(DisplayName="Save Settings"))
class MYPROJECT2_API USaveGameSettings : public UObject
{
	GENERATED_BODY()

public:

	//The id that a save must have to be valid, use this to hard wipe saves
	UPROPERTY(EditAnywhere, Config, Category=Save)
	FGuid SaveId;

	//When using request save how long to wait before performing the save (Allows for multiple save calls in a timeframe only resulting in one save)
	UPROPERTY(EditAnywhere, Config, Category = Save)
	float SaveTime;

	UPROPERTY(EditAnywhere, Config, Category = Save)
	FString GameSaveName;

	UPROPERTY(EditAnywhere, Config, Category = Save)
	FString UserSaveName;
};

UCLASS(BlueprintType, Blueprintable, Abstract)
class MYPROJECT2_API USaveData : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent)
		void OnSave();
	UFUNCTION(BlueprintImplementableEvent)
		void OnLoad();

	UFUNCTION(BlueprintCallable)
	FString ConvertStringToHash(FString InString);

public:
	UPROPERTY()
	int Version;
};

UCLASS()
class MYPROJECT2_API USaveGameInstance : public USaveGame
{
	GENERATED_BODY()

public:
	template <class T=USaveData>
	T* GetOrCreateSaveData()
	{
		return Cast<T>(GetOrCreateSaveData(T::StaticClass()));
	};

	USaveData* GetOrCreateSaveData(UClass* SaveDataClass)
	{
		for (int i = 0; i < SaveData.Num(); i++)
		{
			USaveData* curSaveData = SaveData[i];

			if(curSaveData == nullptr)
				continue;

			if (curSaveData->GetClass() != SaveDataClass)
				continue;

			return curSaveData;
		}

		USaveData* newSaveData = NewObject<USaveData>(this, SaveDataClass);

		SaveData.AddUnique(newSaveData);

		return newSaveData;
	};

	void Serialize(FArchive& Ar);

	void OnSave();

	void OnLoad();

public:
	UPROPERTY()
	FGuid SaveId;
	UPROPERTY()
	FDateTime SaveTime;

	UPROPERTY()
	TArray<USaveData*> SaveData;
};

/**
 * 
 */
UCLASS()
class MYPROJECT2_API USaveSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION(BlueprintCallable)
	void RequestSaveUserInstance();

	UFUNCTION(BlueprintCallable)
	void RequestSaveGameInstance();

	UFUNCTION(BlueprintCallable)
	void ForceSaveUserInstance();

	UFUNCTION(BlueprintCallable)
	void ForceSaveGameInstance();

	UFUNCTION(BlueprintCallable)
	void LoadUserSaveInstance(FString InUserId);

	UFUNCTION(BlueprintCallable)
	USaveData* GetUserSaveData(TSubclassOf<USaveData> SaveDataType);

	UFUNCTION(BlueprintCallable)
	USaveData* GetGameSaveData(TSubclassOf<USaveData> SaveDataType);

	UFUNCTION(BlueprintCallable)
	FString ConvertStringToHash(FString InString);

private:

	USaveData* GetSaveData(USaveGameInstance* SaveInstance, TSubclassOf<USaveData> SaveDataType);

	USaveGameInstance* GetUserSaveInstance();

	USaveGameInstance* GetGameSaveInstance();

	USaveGameInstance* CreateNewSaveInstance();

	USaveGameInstance* LoadSaveInstance(FString SaveName);

	UFUNCTION()
	void SaveGameAsync(USaveGameInstance* SaveInstance, FString SaveName);

	void SaveGameImmediate(USaveGameInstance* SaveInstance, FString SaveName);
private:

	//This stores the users specific save data & is loaded after login
	UPROPERTY()
	USaveGameInstance* m_userSave;
	//This stores general game save data & is loaded on start
	UPROPERTY()
	USaveGameInstance* m_gameSave;

	FString m_userSaveName;

	FTimerHandle m_userSaveTimerHndl;
	FTimerHandle m_gameSaveTimerHndl;
};
