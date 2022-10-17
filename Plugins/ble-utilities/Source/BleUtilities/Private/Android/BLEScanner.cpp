// Copyright (c) 2019 Nineva Studios

#include "BLEScanner.h"
#include "BLEDevice.h"

#include "BleLog.h"
#include "Utils/BLEAndroidUtils.h"

UBLEScanner::UBLEScanner(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer) 
{
	UE_LOG(LogBle, Verbose, TEXT("UBLEScanner::UBLEScanner()"));

	javaScanner = MakeShareable(new FJavaClassObject("com/ninevastudios/androidbleunrealapi/BleUtilitiesScanner",
			"(Landroid/app/Activity;J)V", FJavaWrapper::GameActivityThis, (jlong)this));

	StartScanMethod = javaScanner->GetClassMethod("startScan", "([Ljava/lang/String;)V");
	StopScanMethod = javaScanner->GetClassMethod("stopScan", "()V");
	ConnectToDeviceMethod = javaScanner->GetClassMethod("connect", "(Ljava/lang/String;)V");
	DisconnectDeviceMethod = javaScanner->GetClassMethod("disconnect", "(Ljava/lang/String;)V");
}

void UBLEScanner::PostInitProperties()
{
	Super::PostInitProperties();

	if (UBLEScanner::StaticClass()->GetDefaultObject() != this)
	{
		// add BLE scanner to root to avoid GC issues
		AddToRoot();
	}
}

void UBLEScanner::BeginDestroy()
{
	UE_LOG(LogBle, Verbose, TEXT("UBLEScanner::BeginDestroy()"));

	if (UBLEScanner::StaticClass()->GetDefaultObject() != this)
	{
		// remove BLE scanner from root so now it can be safely destroyed by GC
		RemoveFromRoot();
	}

	Super::BeginDestroy();
}

void UBLEScanner::StartScan(const FOnAdvertisementReceivedDelegate& onAdvertisementReceivedDelegate, const FOnDeviceFoundDelegate& onDeviceFoundCallback, const TArray<FString>& nameFilters)
{
	UE_LOG(LogBle, Verbose, TEXT("UBLEScanner::StartScan()"));

	OnAdvertisementReceivedDelegate = onAdvertisementReceivedDelegate;
	OnDeviceFoundDelegate = onDeviceFoundCallback;

	javaScanner->CallMethod<void>(StartScanMethod, BLEAndroidUtils::GetJavaArray(nameFilters));
}

void UBLEScanner::StopScan()
{
	UE_LOG(LogBle, Verbose, TEXT("UBLEScanner::StopScan()"));

	javaScanner->CallMethod<void>(StopScanMethod);
}

void UBLEScanner::ConnectToDevice(TScriptInterface<IBLEDeviceInterface> device,
	const FOnDeviceConnectedDelegate& onDeviceConnectedCallback, const FOnConnectionErrorDelegate& onFailToConnectDeviceCallback)
{
	UE_LOG(LogBle, Verbose, TEXT("UBLEScanner::ConnectToDevice()"));

	OnDeviceConnectedDelegate = onDeviceConnectedCallback;
	OnConnectionErrorDelegate = onFailToConnectDeviceCallback;

	UBLEDevice* BLEDevice = dynamic_cast<UBLEDevice*>(device.GetObject());
	if (BLEDevice)
	{
		javaScanner->CallMethod<void>(ConnectToDeviceMethod, BLEAndroidUtils::GetJavaString(BLEDevice->GetDeviceId()));
	}
	else
	{
		UE_LOG(LogBle, Error, TEXT("Tried to connect to an invalid device"));
	}
}

void UBLEScanner::DisconnectDevice(TScriptInterface<IBLEDeviceInterface> device,
	const FOnDeviceDisconnectedDelegate& onDeviceDisconnectedCallback, const FOnDisconnectionErrorDelegate& onFailToDisconnectDeviceCallback)
{
	UE_LOG(LogBle, Verbose, TEXT("BLEUTILS => UBLEScanner::DisconnectDevice()"));

	OnDeviceDisconnectedDelegate = onDeviceDisconnectedCallback;
	OnDisconnectionErrorDelegate = onFailToDisconnectDeviceCallback;

	UBLEDevice* BLEDevice = dynamic_cast<UBLEDevice*>(device.GetObject());

	if (BLEDevice)
	{
		javaScanner->CallMethod<void>(DisconnectDeviceMethod, BLEAndroidUtils::GetJavaString(BLEDevice->GetDeviceId()));
	}
	else
	{
		UE_LOG(LogBle, Error, TEXT("Tried to disconnect from an invalid device"));
	}
}
void UBLEScanner::OnAdvertisementReceived(FString deviceName, const TArray<uint8>& data)
{
	UE_LOG(LogBle, Verbose, TEXT("UBLEScanner::OnAdvertisementReceived()"));

	const FString dataString = BytesToHex(data.GetData(), data.Num());
	UE_LOG(LogBle, Verbose, TEXT("Device Name: %s\n\tData: %s"), *deviceName, *dataString);
	
	OnAdvertisementReceivedDelegate.ExecuteIfBound(deviceName, data);
}

void UBLEScanner::OnDeviceFound(UBLEDevice* device)
{
	UE_LOG(LogBle, Verbose, TEXT("UBLEScanner::OnDeviceFound()"));

	//device->SetManagerInternal(Guid);

	OnDeviceFoundDelegate.ExecuteIfBound(device);
}

void UBLEScanner::OnDeviceConnected()
{
	OnDeviceConnectedDelegate.ExecuteIfBound();
}

void UBLEScanner::OnFailedToConnectDevice(FString error)
{
	OnConnectionErrorDelegate.ExecuteIfBound(error);
}

void UBLEScanner::OnDeviceDisconnected()
{
	OnDeviceDisconnectedDelegate.ExecuteIfBound();
}

void UBLEScanner::OnFailedToDisconnectDevice(FString error)
{
	OnDisconnectionErrorDelegate.ExecuteIfBound(error);
}

#if PLATFORM_ANDROID

JNI_METHOD void Java_com_ninevastudios_androidbleunrealapi_BleUtilitiesScanner_onAdvertisementReceived(JNIEnv* env, jclass clazz, 
	jlong objAddr, jstring name, jbyteArray data)
{
	UE_LOG(LogBle, Verbose, TEXT("Java callback onAdvertisementReceived"));

	FString deviceName = BLEAndroidUtils::GetFString(name);
	TArray<uint8> bytes = BLEAndroidUtils::ConvertToByteArray(data);

	UBLEScanner* scanner = reinterpret_cast<UBLEScanner*>(objAddr);

	AsyncTask(ENamedThreads::GameThread, [=]() {
		scanner->OnAdvertisementReceived(deviceName, bytes);
	});
}

JNI_METHOD void Java_com_ninevastudios_androidbleunrealapi_BleUtilitiesScanner_onNewDeviceDiscovered(JNIEnv* env, jclass clazz,
	jlong objAddr, jobject jDevice)
{
	UE_LOG(LogBle, Verbose, TEXT("Java callback onNewDeviceDiscovered"));

	if (!pManagerHolder)
	{
		return;
	}

	UBLEScanner* scanner = reinterpret_cast<UBLEScanner*>(objAddr);

	UBLEDevice* BLEDevice = NewObject<UBLEDevice>();
	BLEDevice->SetDeviceInternal(jDevice, "0");
	TScriptInterface<IBLEDeviceInterface> BLEDeviceInterface;
	BLEDeviceInterface.SetObject(BLEDevice);
	BLEDeviceInterface.SetInterface(Cast<IBLEDeviceInterface>(BLEDevice));

	AsyncTask(ENamedThreads::GameThread, [=]() {
		scanner->OnDeviceFound(BLEDevice);
		});
}

JNI_METHOD void Java_com_ninevastudios_androidbleunrealapi_BleUtilitiesDevice_onDeviceConnected(JNIEnv* env, jclass clazz, jstring guid)
{
	UE_LOG(LogBle, Verbose, TEXT("Java callback onNewDeviceDiscovered"));

	if (!pManagerHolder)
	{
		return;
	}

	FString strGuid = BLEAndroidUtils::GetFString(guid);

	UBLEScanner* scanner = reinterpret_cast<UBLEScanner*>(FCString::Strtoui64(*strGuid, NULL, 10));

	scanner->OnDeviceConnected();
}

JNI_METHOD void Java_com_ninevastudios_androidbleunrealapi_BleUtilitiesDevice_onDeviceDisconnected(JNIEnv* env, jclass clazz, jstring guid)
{
	UE_LOG(LogBle, Verbose, TEXT("Java callback onNewDeviceDiscovered"));

	if (!pManagerHolder)
	{
		return;
	}

	FString strGuid = BLEAndroidUtils::GetFString(guid);

	UBLEScanner* scanner = reinterpret_cast<UBLEScanner*>(FCString::Strtoui64(*strGuid, NULL, 10));

	scanner->OnDeviceDisconnected();
}
#endif
