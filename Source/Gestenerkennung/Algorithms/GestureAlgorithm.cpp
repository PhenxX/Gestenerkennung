// Fill out your copyright notice in the Description page of Project Settings.

#include "GestureAlgorithm.h"

UGestureAlgorithm::UGestureAlgorithm()
{
	Restart();
}

UGestureAlgorithm::~UGestureAlgorithm()
{
}

FGesture * UGestureAlgorithm::GetActualGesture()
{
	return &ActualGesture;
}

void UGestureAlgorithm::Initialize(TArray<FGesture>* _AllGestures)
{
	AllGestures = _AllGestures;
	InitialForwardVector = FVector::ZeroVector;
	InitialRightVector = FVector::ZeroVector;
	InitialUpVector = FVector::ZeroVector;
	Restart();
}

void UGestureAlgorithm::Restart()
{
	ActualGesture = FGesture();
	InitialForwardVector = FVector::ZeroVector;
	InitialRightVector = FVector::ZeroVector;
	InitialUpVector = FVector::ZeroVector;
}

void UGestureAlgorithm::RecordGesture(FVector _InputPosition)
{	
	ActualGesture.SetLocalAxis(InitialRightVector, -InitialForwardVector, InitialUpVector);
	ActualGesture.AddObservation(_InputPosition);
}

FGesture *  UGestureAlgorithm::StopRecordingGesture()
{
	ActualGesture.SpaceOutPoints();
	ActualGesture.CalculateDotProduct();
	return &ActualGesture;
}
