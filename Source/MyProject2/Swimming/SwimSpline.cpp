// Fill out your copyright notice in the Description page of Project Settings.


#include "../Swimming/SwimSpline.h"
#include "../Runtime/Engine/Classes/Components/SplineComponent.h"

// Sets default values
ASwimSpline::ASwimSpline()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>("Root");
	SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
	SplineComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
}

// Called when the game starts or when spawned
void ASwimSpline::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASwimSpline::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

