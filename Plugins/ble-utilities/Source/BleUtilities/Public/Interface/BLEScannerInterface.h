// Copyright (c) 2019 Nineva Studios

#pragma once

#include "UObject/Interface.h"
#include "BLEManagerInterface.h"
#include "BLEScannerInterface.generated.h"

DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnAdvertisementReceivedDelegate, FString, Name, const TArray<uint8>&, data);
//DECLARE_DYNAMIC_DELEGATE_OneParam(FOnDeviceFoundDelegate, TScriptInterface<IBLEDeviceInterface>, device);
//DECLARE_DYNAMIC_DELEGATE(FOnDeviceConnectedDelegate);
//DECLARE_DYNAMIC_DELEGATE(FOnDeviceDisconnectedDelegate);
//DECLARE_DYNAMIC_DELEGATE_OneParam(FOnConnectionErrorDelegate, FString, error);
//DECLARE_DYNAMIC_DELEGATE_OneParam(FOnDisconnectionErrorDelegate, FString, error);

UINTERFACE(BlueprintType, meta = (CannotImplementInterfaceInBlueprint))
class BLEUTILITIES_API UBLEScannerInterface : public UInterface
{
	GENERATED_BODY()
};

class BLEUTILITIES_API IBLEScannerInterface
{
	GENERATED_BODY()

public:

	/**
	* Start Scan
	*/
	UFUNCTION(BlueprintCallable, Category = "BleUtilities", meta = (AutoCreateRefTerm = "nameFilters"))
	virtual void StartScan(const FOnAdvertisementReceivedDelegate& onAdvertisementReceivedDelegate, const FOnDeviceFoundDelegate& onDeviceFoundCallback,
		const TArray<FString>& nameFilters) = 0;

	/**
	* Stop Scan
	*/
	UFUNCTION(BlueprintCallable, Category = "BleUtilities")
	virtual void StopScan() = 0;

	/**
* Connect to specified BLE device
*/
	UFUNCTION(BlueprintCallable, Category = "BleUtilities")
		virtual void ConnectToDevice(TScriptInterface<IBLEDeviceInterface> device,
			const FOnDeviceConnectedDelegate& onDeviceConnectedCallback, const FOnConnectionErrorDelegate& onFailToConnectDeviceCallback) = 0;

	/**
	* Disconnect specified BLE device
	*/
	UFUNCTION(BlueprintCallable, Category = "BleUtilities")
		virtual void DisconnectDevice(TScriptInterface<IBLEDeviceInterface> device,
			const FOnDeviceDisconnectedDelegate& onDeviceDisconnectedCallback, const FOnDisconnectionErrorDelegate& onFailToDisconnectDeviceCallback) = 0;
};
