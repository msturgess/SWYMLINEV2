// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WorkoutDataAsset.h"
#include "WorkoutActor.generated.h"

class ASwimSpline;

UCLASS(Blueprintable, BlueprintType)
class MYPROJECT2_API AWorkoutActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWorkoutActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void StartSession();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Workout")
	void StartWorkout(UWorkoutDataAsset* InWorkout, AActor* InSwimmer);

	UFUNCTION(BlueprintCallable, Category = "Workout")
	void StopWorkout();

	UFUNCTION(BlueprintCallable, Category = "Workout")
	void StartNextSession();

	UFUNCTION(BlueprintCallable, Category = "Workout")
	void PauseWorkout();

	UFUNCTION(BlueprintCallable, Category = "Workout")
	void ResumeWorkout();

	UFUNCTION(BlueprintCallable, Category = "Workout")
	void LapComplete();

	UFUNCTION(BlueprintImplementableEvent, Category = "Workout")
	void OnWorkoutStart();

	UFUNCTION(BlueprintImplementableEvent, Category = "Workout")
	void OnRestStart();

	UFUNCTION(BlueprintImplementableEvent, Category = "Workout")
	void OnRestFinish();

	UFUNCTION(BlueprintImplementableEvent, Category = "Workout")
	void OnWorkoutFinished();

	UFUNCTION(BlueprintImplementableEvent, Category = "Workout")
	void OnWorkoutSessionStart();

	UFUNCTION(BlueprintImplementableEvent, Category = "Workout")
	void OnWorkoutSessionFinished();

	UFUNCTION(BlueprintCallable, Category = "Workout")
	void SetupSwimmer(bool Forward);

	UFUNCTION(BlueprintCallable, Category = "Workout")
	FTransform GetSwimmerTransform(AActor* InSwimmer, float MoveDist, bool Forward);

	UFUNCTION(BlueprintCallable, Category = "Workout")
	bool IsResting();

	UFUNCTION(BlueprintCallable, Category = "Workout")
	bool IsPaused();

	UFUNCTION(BlueprintCallable, Category = "Workout")
	int GetMaxLaps() const;

	UFUNCTION(BlueprintCallable, Category = "Workout")
	int GetCurrentLap() const;

	UFUNCTION(BlueprintCallable, Category = "Workout")
	float GetSessionTimer() const;

	UFUNCTION(BlueprintCallable, Category = "Workout")
	float GetWorkoutTimer() const;

	UFUNCTION(BlueprintCallable, Category = "Workout")
	float GetSessionDistance() const;

	UFUNCTION(BlueprintCallable, Category = "Workout")
	float GetWorkoutDistance() const;

	UFUNCTION(BlueprintCallable, Category = "Workout")
	float GetRestTimer() const;

	UFUNCTION(BlueprintCallable, Category = "Workout")
	FWorkoutSession& GetCurrentSession();
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Workout")
	ASwimSpline* DefaultSpline;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Workout")
	TMap<UWorkoutDataAsset*, ASwimSpline*> WorkoutSpline;

	UPROPERTY()
	UWorkoutDataAsset* ActiveWorkout;

	UPROPERTY(BlueprintReadOnly)
	ASwimSpline* ActiveSpline;

	UPROPERTY()
	AActor* Swimmer;
private:
	FWorkoutSession* m_workoutSession;
	int m_sessionIdx;
	float m_restTimer;
	float m_sessionTimer;
	float m_workoutTimer;

	float m_sessionDistance;
	float m_workoutDistance;
	float m_lastDistance;

	bool m_active;
	bool m_sessionFinished;
	bool m_wasSwimmingFoward;
	bool m_isSwimmingForward;

	int m_curLap;

	float m_sessionTime;
};
