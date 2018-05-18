// Fill out your copyright notice in the Description page of Project Settings.

#include "CrossProductAlgorithm.h"

UCrossProductAlgorithm::UCrossProductAlgorithm()
{
}

void UCrossProductAlgorithm::Initialize(TArray<FGesture>* _AllGestures)
{
	Super::Initialize(_AllGestures);
}

TArray<FGestureFeedback> UCrossProductAlgorithm::CalculateGestureFeedback(TArray<int32> _GestureIndices, FVector _InputPosition)
{
	TArray<FGestureFeedback> GestureFeedback =TArray<FGestureFeedback>();
	GesturesToObserv.Empty();

	if (AllGestures->IsValidIndex(0))
	{
		for (int32 GestureIndex : _GestureIndices)
		{
			GesturesToObserv.Add(&((*AllGestures)[GestureIndex]));
		}
	}
	ActualGesture.SpaceOutPoints();
	int32 NumOfPoints = ActualGesture.NormalizedSpacedInput.Num();
	ActualGesture.CalculateDotProduct();

	for (FGesture* TmpGesture : GesturesToObserv)
	{
		if (TmpGesture->NormalizedSpacedInput.Num() != NumOfPoints) continue;
		
		float Likelihood = 0.f;

		for (int i = 0; i < NumOfPoints; i++)
		{
			Likelihood += (TmpGesture->NormalizedSpacedInput[i].X * ActualGesture.NormalizedSpacedInput[i].X) + (TmpGesture->NormalizedSpacedInput[i].Y * ActualGesture.NormalizedSpacedInput[i].Y) + (TmpGesture->NormalizedSpacedInput[i].Z * ActualGesture.NormalizedSpacedInput[i].Z);
		}

		Likelihood /= sqrtf(TmpGesture->DotProductSelf * ActualGesture.DotProductSelf);
		GestureFeedback.Add(FGestureFeedback(TmpGesture->GestureID, Likelihood, 1.f));
	}

	GestureFeedback.Sort();

	return GestureFeedback;
}

void UCrossProductAlgorithm::RecordGesture(FVector _InputPosition)
{
	Super::RecordGesture(_InputPosition);
}

FGesture * UCrossProductAlgorithm::StopRecordingGesture()
{	
	return Super::StopRecordingGesture();
}

void UCrossProductAlgorithm::Restart()
{
	Super::Restart();
}
