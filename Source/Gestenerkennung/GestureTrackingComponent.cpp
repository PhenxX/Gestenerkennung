// Fill out your copyright notice in the Description page of Project Settings.
#include "GestureTrackingComponent.h"
#include "EngineUtils.h"




// Sets default values for this component's properties
UGestureTrackingComponent::UGestureTrackingComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UGestureTrackingComponent::BeginPlay()
{
	Super::BeginPlay();
	// ...
	
}


// Called every frame
void UGestureTrackingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

FVector UGestureTrackingComponent::CalcRelativeLocation()
{

	return  this->GetComponentLocation();
}

void UGestureTrackingComponent::SetToLeftHand()
{
	this->AddWorldRotation(FRotator(0,0,180));
}


