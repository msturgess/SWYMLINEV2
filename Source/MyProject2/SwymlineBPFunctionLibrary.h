// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SwymlineBPFunctionLibrary.generated.h"

USTRUCT(BlueprintType, Blueprintable)
struct FSwimInputData
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="DeviceInput")
	FVector Accelerometer;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DeviceInput")
	FVector Gyrometer;
};
/**
 * 
 */
UCLASS()
class MYPROJECT2_API USwymlineBPFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:

	//Byte array should be 6 consecutive floats, first 3 Accelerometer, the last 3 Gyrometer
	UFUNCTION(BlueprintCallable)
	static bool ConvertBytesToInput(const TArray<uint8>& InBytes, FSwimInputData& OutInputData);

	//string should be 6 comma separated floats, first 3 Accelerometer, the last 3 Gyrometer
	UFUNCTION(BlueprintCallable)
	static bool ConvertStringToInput(FString InData, FSwimInputData& OutInputData);

	UFUNCTION(BlueprintCallable)
	static bool ConvertByteStringToInput(const TArray<uint8>& InBytes, FSwimInputData& OutInputData);

	UFUNCTION(BlueprintCallable)
	static TArray<uint8> ConvertStringToBytes(FString InString);

	UFUNCTION(BlueprintCallable)
	static FString ConvertByteStringToString(FString InString);

	UFUNCTION(BlueprintCallable)
	static FString ConvertByteStringsToString(const TArray<FString>& InStrings);
};
