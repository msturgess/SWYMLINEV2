// Fill out your copyright notice in the Description page of Project Settings.


#include "SwymlineBPFunctionLibrary.h"
#include <iostream>
#include <string>

static TArray<float> CovertBytesToFloats(TArray<uint8> Bytes)
{
	//If its a float array it should be divisible by 4
	if (Bytes.Num() % 4 != 0)
	{
		return TArray<float>();
	}

	uint8* byteArr = new uint8[4];
	TArray<float> outFloats;

	for (int i = 0; i < Bytes.Num(); i += 4)
	{
		byteArr[0] = Bytes[i];
		byteArr[1] = Bytes[i + 1];
		byteArr[2] = Bytes[i + 2];
		byteArr[3] = Bytes[i + 3];

		outFloats.Add(*reinterpret_cast<float*>(byteArr));
	}

	return outFloats;
}

bool USwymlineBPFunctionLibrary::ConvertBytesToInput(const TArray<uint8>& InBytes, FSwimInputData& OutInputData)
{
	TArray<float> floatData = CovertBytesToFloats(InBytes);

	if (floatData.Num() < 6)
	{
		UE_LOG(LogTemp, Warning, TEXT("floatData is less than the expected amount %i out of 6"), floatData.Num());
		return false;
	}

	if (floatData.Num() > 0)
		OutInputData.Accelerometer.X = floatData[0];
	if (floatData.Num() > 1)
		OutInputData.Accelerometer.Y = floatData[1];
	if (floatData.Num() > 2)
		OutInputData.Accelerometer.Z = floatData[2];

	
	if (floatData.Num() > 3)
		OutInputData.Gyrometer.X = floatData[3];
	if (floatData.Num() > 4)
		OutInputData.Gyrometer.Y = floatData[4];
	if (floatData.Num() > 5)
		OutInputData.Gyrometer.Z = floatData[5];

	return true;
}

bool USwymlineBPFunctionLibrary::ConvertStringToInput(FString InData, FSwimInputData& OutInputData)
{
	TArray<float> floatData;

	FString curData = "";
	bool hasNumber = false;

	for (int i = 0; i < InData.Len(); i++)
	{
		if (InData[i] == ',')
		{
			hasNumber = false;

			for (int j = 0; j < curData.Len(); j++)
			{
				if (!TChar<TCHAR>::IsDigit(curData[j]))
					continue;
				hasNumber = true;
				break;
			}

			if (!hasNumber)
				curData = "";

			floatData.Add(curData.IsEmpty() ? 0.0f : std::stof(*curData));
			curData = "";
			continue;
		}

		TCHAR& curChar = InData[i];
		
		if(!TChar<TCHAR>::IsDigit(curChar) && !TChar<TCHAR>::IsPunct(curChar))
			continue;

		curData += curChar;
	}

	hasNumber = false;

	for (int i = 0; i < curData.Len(); i++)
	{
		if (!TChar<TCHAR>::IsDigit(curData[i]))
			continue;
		hasNumber = true;
		break;
	}

	if (!hasNumber)
		curData = "";

	UE_LOG(LogTemp, Display, TEXT("LastInput: %s"), *curData);

	floatData.Add(curData.IsEmpty() ? 0.0f : std::stof(*curData));

	if (floatData.Num() < 6)
		return false;

	OutInputData.Accelerometer.X = floatData[0];
	OutInputData.Accelerometer.Y = floatData[1];
	OutInputData.Accelerometer.Z = floatData[2];

	OutInputData.Gyrometer.X = floatData[3];
	OutInputData.Gyrometer.Y = floatData[4];
	OutInputData.Gyrometer.Z = floatData[5];

	return true;
}

FString BytesToStringFixed(const uint8* In, int32 Count)
{
	FString Broken = BytesToString(In, Count);
	FString Fixed;

	for (int i = 0; i < Broken.Len(); i++)
	{
		const TCHAR c = Broken[i] - 1;
		Fixed.AppendChar(c);
	}

	return Fixed;
}

FString ByteArrToString(const TArray<uint8>& InBytes)
{
	FString output;

	for (int i = 0; i < InBytes.Num(); i++)
	{
		output += TCHAR(InBytes[i]);
	}

	return output;
}

bool USwymlineBPFunctionLibrary::ConvertByteStringToInput(const TArray<uint8>& InBytes, FSwimInputData& OutInputData)
{
	if (InBytes.Num() <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Inbytes is empty"));
		return false;
	}

	UE_LOG(LogTemp, Display, TEXT("TotalBytes: %i"), InBytes.Num());

	//uint8* byteArr = new uint8[InBytes.Num()];
	//
	//for (int i = 0; i < InBytes.Num(); i++)
	//{
	//	byteArr[i] = InBytes[i];
	//}

	FString byteAsStr = ByteArrToString(InBytes);

	UE_LOG(LogTemp, Display, TEXT("Length: %i, Data: %s"), byteAsStr.Len(), *byteAsStr);

	bool success = ConvertStringToInput(byteAsStr, OutInputData);

	UE_LOG(LogTemp, Display, TEXT("Conversion okay"));

	return success;
}

TArray<uint8> USwymlineBPFunctionLibrary::ConvertStringToBytes(FString InString)
{
	int arrSize = InString.Len();
	uint8* byteArr = new uint8[arrSize];

	TArray<uint8> outBytes;

	if (!FString::ToBlob(InString, byteArr, arrSize))
	{
		delete byteArr;
		return outBytes;
	}

	for (int i = 0; i < arrSize; i++)
	{
		outBytes.Add(byteArr[i]);
	}

	delete byteArr;
	return outBytes;
}

FString USwymlineBPFunctionLibrary::ConvertByteStringToString(FString InString)
{
	TArray<uint8> bytes = ConvertStringToBytes(InString);

	if (bytes.Num() <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Inbytes is empty"));
		return FString();
	}

	//UE_LOG(LogTemp, Display, TEXT("TotalBytes: %i"), InBytes.Num());

	uint8* byteArr = new uint8[bytes.Num()];

	for (int i = 0; i < bytes.Num(); i++)
	{
		byteArr[i] = bytes[i];
	}

	FString byteAsStr = BytesToStringFixed(byteArr, sizeof(byteArr));

	delete byteArr;

	return byteAsStr;
}

FString USwymlineBPFunctionLibrary::ConvertByteStringsToString(const TArray<FString>& InStrings)
{
	FString output;

	for (int i = 0; i < InStrings.Num(); i++) 
	{
		output += ConvertByteStringToString(InStrings[i]);
		output += "  -  ";
	}

	UE_LOG(LogTemp, Display, TEXT("%s"), *output);

	return output;
}