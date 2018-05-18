// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GestureUtils.h"
#include <EngineGlobals.h>
#include <Runtime/Engine/Classes/Engine/Engine.h>
#include "GestureAlgorithm.generated.h"

/**
 * 
 */
UCLASS(abstract)
class GESTENERKENNUNG_API UGestureAlgorithm : public UObject
{
	GENERATED_BODY()

protected:

	FGesture ActualGesture;
	TArray<FGesture*> GesturesToObserv;

public:

	TArray<FGesture>* AllGestures;
	FVector InitialForwardVector;
	FVector InitialRightVector;
	FVector InitialUpVector;

	UGestureAlgorithm();	
	~UGestureAlgorithm();

	FGesture* GetActualGesture();

	virtual void Initialize(TArray<FGesture>* _AllGestures);

	/**
	* Calculates the feedback values acording to the input parameters. Feedback will be persistent until Restart() is called.
	* @param GestureIndices an Array of Integer holding the indices of the necessary gestures
	* @param InputPosition the actual position relative to the player
	* @return TArray<FGestureFeedback> the calculated results for each observed gesture
	**/
	virtual TArray<FGestureFeedback> CalculateGestureFeedback(TArray<int32> _GestureIndices, FVector _InputPosition) PURE_VIRTUAL(UGestureAlgorithm::CalculateGestureFeedback, return TArray<FGestureFeedback>(););
	
	virtual void RecordGesture(FVector _InputPosition);// PURE_VIRTUAL(UGestureAlgorithm::RecordeGesture, return &(FGesture()););

	virtual FGesture* StopRecordingGesture();

	//Reset inner states, should be called before start analyzing a new gesture
	virtual void Restart();	
};

