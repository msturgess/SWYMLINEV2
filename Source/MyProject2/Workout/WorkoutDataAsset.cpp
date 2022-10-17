// Fill out your copyright notice in the Description page of Project Settings.


#include "../Workout/WorkoutDataAsset.h"

FWorkoutSession::FWorkoutSession()
	: WorkoutType(EWorkoutType::WT_Laps),
	Laps(3),
	DefaultBestLapTime(60.0f),
	DefaultBestTotalTime(180.0f),
	WorkoutTime(600.0f)
{

}

UWorkoutDataAsset::UWorkoutDataAsset()
	: WorkoutId(FGuid::NewGuid())
{

}