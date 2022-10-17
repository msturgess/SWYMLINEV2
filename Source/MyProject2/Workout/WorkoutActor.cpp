// Fill out your copyright notice in the Description page of Project Settings.


#include "../Workout/WorkoutActor.h"
#include "../Swimming/SwimSpline.h"
#include "../Runtime/Engine/Classes/Components/SplineComponent.h"

// Sets default values
AWorkoutActor::AWorkoutActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AWorkoutActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AWorkoutActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!m_active)
		return;

	if (!ActiveWorkout)
		return;

	m_workoutTimer += DeltaTime;
	m_sessionTimer += DeltaTime;

	if (m_restTimer > 0.0f)
	{
		m_restTimer -= DeltaTime;

		if (m_restTimer < 0.0f)
		{
			m_restTimer = 0.0f;
			OnRestFinish();
		}

		return;
	}
	else if(m_sessionFinished)
	{
		OnWorkoutSessionFinished();

		if (m_sessionIdx >= ActiveWorkout->WorkoutSessions.Num()-1)
		{
			OnWorkoutFinished();
			StopWorkout();
			return;
		}

		StartNextSession();
	}

	if (Swimmer)
	{
		float splineLength = ActiveSpline->SplineComponent->GetSplineLength();

		float splineProg = ActiveSpline->SplineComponent->FindInputKeyClosestToWorldLocation(Swimmer->GetActorLocation());

		float curSplineDist = ActiveSpline->SplineComponent->GetDistanceAlongSplineAtSplineInputKey(splineProg);

		float moveAmount = 0.0f;

		if (m_isSwimmingForward && !m_wasSwimmingFoward)
		{
			moveAmount += curSplineDist;
			moveAmount += m_lastDistance;
		}
		else if (!m_isSwimmingForward && m_wasSwimmingFoward)
		{
			moveAmount += splineLength - curSplineDist;
			moveAmount += splineLength - m_lastDistance;
		}
		else
		{
			if (m_isSwimmingForward)
			{
				if (curSplineDist >= m_lastDistance)
				{
					moveAmount += curSplineDist - m_lastDistance;
				}
				else if(m_lastDistance - curSplineDist > splineLength * 0.5f)
				{
					moveAmount += splineLength - m_lastDistance;
					moveAmount += curSplineDist;
				}
			}
			else
			{
				if (curSplineDist <= m_lastDistance)
				{
					moveAmount += m_lastDistance - curSplineDist;
				}
				else if(curSplineDist - m_lastDistance > splineLength * 0.5f)
				{
					moveAmount += m_lastDistance;
					moveAmount += splineLength - curSplineDist;
				}
			}
		}

		//Distance is in meters, 1UU = 1cm
		m_workoutDistance += moveAmount / 100.0f;
		m_sessionDistance += moveAmount / 100.0f;

		m_lastDistance = curSplineDist;

		m_wasSwimmingFoward = m_isSwimmingForward;
	}

	if (m_workoutSession->WorkoutType == EWorkoutType::WT_Time)
	{
		if (m_sessionTimer >= m_workoutSession->WorkoutTime)
		{
			m_sessionTimer = m_workoutSession->WorkoutTime;
			m_restTimer = m_workoutSession->RestTime;
			m_sessionFinished = true;

			if (m_restTimer > 0.0f)
				OnRestStart();
		}
	}

	if (m_workoutSession->WorkoutType == EWorkoutType::WT_Distance)
	{
		if (m_workoutDistance >= m_workoutSession->WorkoutDistance)
		{
			m_workoutDistance = m_workoutSession->WorkoutDistance;
			m_restTimer = m_workoutSession->RestTime;
			m_sessionFinished = true;

			if (m_restTimer > 0.0f)
				OnRestStart();
		}
	}
}

void AWorkoutActor::StartSession()
{
	if (!ActiveWorkout)
		return;

	m_workoutSession = &ActiveWorkout->WorkoutSessions[m_sessionIdx];
	m_curLap = 0;
	m_sessionTimer = 0.0f;
	m_sessionDistance = 0.0f;
	m_sessionFinished = false;

	OnWorkoutSessionStart();
}

void AWorkoutActor::StartNextSession()
{
	if (!ActiveWorkout)
		return;

	if (m_sessionIdx >= ActiveWorkout->WorkoutSessions.Num()-1)
		return;

	m_sessionIdx++;
	
	StartSession();
}

void AWorkoutActor::StartWorkout_Implementation(UWorkoutDataAsset* InWorkout, AActor* InSwimmer)
{
	//Already active
	if (m_active)
		return;

	ActiveWorkout = InWorkout;
	Swimmer = InSwimmer;
	m_active = true;
	m_sessionIdx = 0;
	m_sessionTimer = 0.0f;
	m_workoutTimer = 0.0f;
	m_restTimer = 0.0f;
	m_workoutDistance = 0.0f;

	if (WorkoutSpline.Contains(ActiveWorkout))
	{
		ActiveSpline = WorkoutSpline[ActiveWorkout];
	}
	else
	{
		ActiveSpline = DefaultSpline;
	}

	StartSession();

	OnWorkoutStart();
}

void AWorkoutActor::StopWorkout()
{
	if (!ActiveWorkout)
		return;

	m_active = false;
	ActiveWorkout = nullptr;
}

void AWorkoutActor::PauseWorkout()
{
	if (!ActiveWorkout)
		return;

	m_active = false;
}

void AWorkoutActor::ResumeWorkout()
{
	if (!ActiveWorkout)
		return;

	m_active = true;
}

void AWorkoutActor::LapComplete()
{
	if (!ActiveWorkout)
		return;

	m_curLap++;

	if (m_workoutSession->WorkoutType == EWorkoutType::WT_Laps)
	{
		if (m_curLap >= m_workoutSession->Laps)
		{
			m_restTimer = m_workoutSession->RestTime;
			m_sessionFinished = true;

			if (m_restTimer > 0.0f)
				OnRestStart();
		}
	}
}

//
//void AWorkoutActor::OnWorkoutFinished()
//{
//
//}
//
//void AWorkoutActor::OnWorkoutSessionFinished()
//{
//
//}

void AWorkoutActor::SetupSwimmer(bool Forward)
{
	m_wasSwimmingFoward = Forward;
	m_isSwimmingForward = Forward;
	m_workoutDistance = 0.0f;

	float splineProg = ActiveSpline->SplineComponent->FindInputKeyClosestToWorldLocation(Swimmer->GetActorLocation());

	m_lastDistance = ActiveSpline->SplineComponent->GetDistanceAlongSplineAtSplineInputKey(splineProg);
}

FTransform AWorkoutActor::GetSwimmerTransform(AActor* InSwimmer, float MoveDist, bool Forward)
{
	if (!InSwimmer)
		return FTransform::Identity;

	if (!m_active)
		return InSwimmer->GetActorTransform();

	if(ActiveSpline == nullptr)
		return InSwimmer->GetActorTransform();

	//Resting so don't move
	if(m_restTimer > 0.0f)
		return InSwimmer->GetActorTransform();

	float splineLength = ActiveSpline->SplineComponent->GetSplineLength();

	float splineProg = ActiveSpline->SplineComponent->FindInputKeyClosestToWorldLocation(InSwimmer->GetActorLocation());

	float curSplineDist = ActiveSpline->SplineComponent->GetDistanceAlongSplineAtSplineInputKey(splineProg);
	
	if (Forward)
	{
		curSplineDist += MoveDist;

		if (curSplineDist > splineLength)
			curSplineDist = splineLength;
	}
	else
	{
		curSplineDist -= MoveDist;

		if (curSplineDist < 0.0f)
			curSplineDist = 0.0f;
	}

	FTransform splineTrans = ActiveSpline->SplineComponent->GetTransformAtDistanceAlongSpline(curSplineDist, ESplineCoordinateSpace::World);

	FQuat splineQuat = splineTrans.GetRotation();

	//Align to characters look direction
	if (!Forward)
	{
		float currentRotation = PI;

		FQuat rotQuat(FVector(0.0f, 0.0f, 1.0f), currentRotation);
		splineQuat *= rotQuat;
		splineQuat.Normalize();

		splineTrans.SetRotation(splineQuat);
	}

	m_isSwimmingForward = Forward;

	return splineTrans;
}

bool AWorkoutActor::IsResting()
{
	return m_restTimer > 0.0f;
}

bool AWorkoutActor::IsPaused()
{
	return !m_active;
}

int AWorkoutActor::GetMaxLaps() const
{
	if (!ActiveWorkout)
		return 0;

	if (m_workoutSession->WorkoutType != EWorkoutType::WT_Laps)
		return 1;

	return m_workoutSession->Laps;
}

int AWorkoutActor::GetCurrentLap() const
{
	if (!ActiveWorkout)
		return 0;

	return m_curLap;
}

float AWorkoutActor::GetSessionTimer() const
{
	if (!ActiveWorkout)
		return 0.0f;

	return m_sessionTimer;
}

float AWorkoutActor::GetWorkoutTimer() const
{
	if (!ActiveWorkout)
		return 0.0f;

	return m_workoutTimer;
}

float AWorkoutActor::GetSessionDistance() const
{
	if (!ActiveWorkout)
		return 0.0f;

	return m_sessionDistance;
}

float AWorkoutActor::GetWorkoutDistance() const
{
	if (!ActiveWorkout)
		return 0.0f;

	return m_workoutDistance;
}

float AWorkoutActor::GetRestTimer() const
{
	if (!ActiveWorkout)
		return 0.0f;

	return m_restTimer;
}

FWorkoutSession& AWorkoutActor::GetCurrentSession()
{
	return *m_workoutSession;
}