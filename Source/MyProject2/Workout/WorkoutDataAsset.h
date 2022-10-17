// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "WorkoutDataAsset.generated.h"

UENUM(BlueprintType)
enum EWorkoutType
{
	WT_Laps UMETA(DisplayName="Laps"),
	WT_Time UMETA(DisplayName = "Time"),
	WT_Distance UMETA(DisplayName = "Distance")
};

USTRUCT(BlueprintType)
struct FWorkoutSession
{
	GENERATED_BODY()

public:
	FWorkoutSession();

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Workout")
	TEnumAsByte<EWorkoutType> WorkoutType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Workout")
	float RestTime;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Workout", meta=(EditCondition="WorkoutType == EWorkoutType::WT_Laps"))
	int Laps;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Workout", meta = (EditCondition = "WorkoutType == EWorkoutType::WT_Laps"))
	float DefaultBestLapTime;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Workout", meta = (EditCondition = "WorkoutType != EWorkoutType::WT_Time"))
	float DefaultBestTotalTime;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Workout", meta = (EditCondition = "WorkoutType == EWorkoutType::WT_Time"))
	float WorkoutTime;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Workout", meta = (EditCondition = "WorkoutType == EWorkoutType::WT_Distance"))
	float WorkoutDistance;
};

/**
 * 
 */
UCLASS(BlueprintType)
class MYPROJECT2_API UWorkoutDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UWorkoutDataAsset();
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Workout")
	FGuid WorkoutId;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Workout")
	FText WorkoutName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Workout")
	TSoftObjectPtr<UWorld> WorkoutLevel;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Workout")
	TArray< FWorkoutSession> WorkoutSessions;
};
