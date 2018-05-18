// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Algorithms/GestureAlgorithm.h"
#include "CrossProductAlgorithm.generated.h"

/**
 * 
 */
UCLASS()
class GESTENERKENNUNG_API UCrossProductAlgorithm : public UGestureAlgorithm
{
	GENERATED_BODY()

public:

	UCrossProductAlgorithm();

	virtual void Initialize(TArray<FGesture>* _AllGestures) override;

	virtual TArray<FGestureFeedback> CalculateGestureFeedback(TArray<int32> _GestureIndices, FVector _InputPosition) override;

	virtual void RecordGesture(FVector _InputPosition) override;

	virtual FGesture* StopRecordingGesture() override;

	//Reset inner states, should be called before start analyzing a new gesture
	virtual void Restart() override;
};
