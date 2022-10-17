// Copyright (c) 2019 Nineva Studios

#pragma once

#include "Interface/BLEScannerInterface.h"

#include "Android/AndroidJNI.h"
#include "Android/AndroidApplication.h"
#include "Android/AndroidJava.h"

#include "BLEScanner.generated.h"

class UBLEDevice;

UCLASS()
class UBLEScanner : public UObject, public IBLEScannerInterface
{
	GENERATED_UCLASS_BODY()

public:

	virtual ~UBLEScanner() = default;
	
	virtual void PostInitProperties() override;
	virtual void BeginDestroy() override;

	virtual void StartScan(const FOnAdvertisementReceivedDelegate& onAdvertisementReceivedDelegate, const FOnDeviceFoundDelegate& onDeviceFoundCallback,
		const TArray<FString>& nameFilters) override;
	virtual void StopScan() override;

	virtual void ConnectToDevice(TScriptInterface<IBLEDeviceInterface> device,
		const FOnDeviceConnectedDelegate& onDeviceConnectedCallback, const FOnConnectionErrorDelegate& onFailToConnectDeviceCallback) override;

	virtual void DisconnectDevice(TScriptInterface<IBLEDeviceInterface> device,
		const FOnDeviceDisconnectedDelegate& onDeviceDisconnectedCallback, const FOnDisconnectionErrorDelegate& onFailToDisconnectDeviceCallback) override;

	void OnAdvertisementReceived(FString deviceName, const TArray<uint8>& data);


	void OnDeviceFound(UBLEDevice* device);
	void OnDeviceConnected();
	void OnFailedToConnectDevice(FString error);
	void OnDeviceDisconnected();
	void OnFailedToDisconnectDevice(FString error);

private:
	TSharedPtr<FJavaClassObject> javaScanner;

	FJavaClassMethod StartScanMethod;
	FJavaClassMethod StopScanMethod;
	FJavaClassMethod ConnectToDeviceMethod;
	FJavaClassMethod DisconnectDeviceMethod;

	FOnAdvertisementReceivedDelegate OnAdvertisementReceivedDelegate;

	UPROPERTY()
		FOnDeviceFoundDelegate OnDeviceFoundDelegate;
	UPROPERTY()
		FOnDeviceConnectedDelegate OnDeviceConnectedDelegate;
	UPROPERTY()
		FOnConnectionErrorDelegate OnConnectionErrorDelegate;
	UPROPERTY()
		FOnDeviceDisconnectedDelegate OnDeviceDisconnectedDelegate;
	UPROPERTY()
		FOnDisconnectionErrorDelegate OnDisconnectionErrorDelegate;
};
