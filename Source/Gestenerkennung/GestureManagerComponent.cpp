// Fill out your copyright notice in the Description page of Project Settings.

#include "GestureManagerComponent.h"
#include <string>
#include "FileHelper.h"
#include "FileManagerGeneric.h"

void UGestureManagerComponent::Initialise(UGestureTrackingComponent* _LeftTrackingComponent, UGestureTrackingComponent* _RightTrackingComponent, FString _Path)
{
	LeftTrackingComponent = _LeftTrackingComponent;
	LeftTrackingComponent->SetToLeftHand();
	RightTrackingComponent = _RightTrackingComponent;
	HandToTrack = EHandToTrack::HTT_Left;
	DataLogger = NewObject<UDataLogger>();
	DataLogger->Initilize(FPaths::GameDir() + "\\GestureLogs", "CrossValues.txt", true);
	GestureRecognitionState = EGestureRecognitionState::GRS_Idle;
	SwitchAlgorithm(AlgorithmToUse);
	LastUsedGestureID = 0;
	IndicesToUse = TArray<int32>();
	LoadGesturesFromFile(FPaths::GameDir() + _Path);
}

FGesture UGestureManagerComponent::GetGestureByID(int32 _GestureIDToFind)
{
	for (FGesture TmpGesture : AllGestures)
	{
		if (TmpGesture.GestureID == _GestureIDToFind) return TmpGesture;
	}
	return FGesture();
}

void UGestureManagerComponent::ClearGestures()
{
	AllGestures.Empty();
	LastUsedGestureID = 0;
	IndicesToUse.Empty();
	Algorithm->Restart();
	OnGesturesCleared.Broadcast();
}

UDataLogger* UGestureManagerComponent::GetDataLogger()
{
	return DataLogger;
}

void UGestureManagerComponent::LoadGesturesFromFile(FString _Path)
{		
	int32 i = 1;
	FString sName = "Gesture";
	FString s = _Path + '\\' + sName + ".txt";		
	FFileHelper FileHelper = FFileHelper();
	FString Result;


	while (FileHelper.LoadFileToString(Result, *s, FFileHelper::EHashOptions::None))
	{
		sName = "Gesture" + FString::FromInt(i);
		++i;
		s = _Path + '\\' + sName + ".txt";
		FString TmpString;
		FVector TmpPoint;
		int32 Count = 1;
	/*	Algorithm->InitialForwardVector = FVector(-0.863034, 0.491209, -0.117841);
		Algorithm->InitialRightVector = FVector(0.41327, 0.820731, 0.394472);
		Algorithm->InitialUpVector = FVector(-0.290483, -0.291743, 0.911321);*/
		//SetAlgorithmAxis();
		int first, second, third;
		FVector X, Y, Z;
		for (int i = 0; i < 3; i++)
		{
			if (i == 0)
			{
				first = Result.Find(";", ESearchCase::IgnoreCase, ESearchDir::FromStart, -1);
				second = Result.Find(";", ESearchCase::IgnoreCase, ESearchDir::FromStart, first + 1);
				third = Result.Find(";", ESearchCase::IgnoreCase, ESearchDir::FromStart, second + 1);

				X = FVector(FCString::Atof(*(Result.Mid(-1, first - 1))), FCString::Atof(*(Result.Mid(first + 1, second - 1))), FCString::Atof(*(Result.Mid(second + 1, third - 1))));
			}

			else if (i == 1)
			{
				first = Result.Find(";", ESearchCase::IgnoreCase, ESearchDir::FromStart, third + 1);
				second = Result.Find(";", ESearchCase::IgnoreCase, ESearchDir::FromStart, first + 1);
				third = Result.Find(";", ESearchCase::IgnoreCase, ESearchDir::FromStart, second + 1);

				Y = FVector(FCString::Atof(*(Result.Mid(-1, first - 1))), FCString::Atof(*(Result.Mid(first + 1, second - 1))), FCString::Atof(*(Result.Mid(second + 1, third - 1))));
			}

			else if (i == 2)
			{
				first = Result.Find(";", ESearchCase::IgnoreCase, ESearchDir::FromStart, third + 1);
				second = Result.Find(";", ESearchCase::IgnoreCase, ESearchDir::FromStart, first + 1);
				third = Result.Find(";", ESearchCase::IgnoreCase, ESearchDir::FromStart, second + 1);

				Z = FVector(FCString::Atof(*(Result.Mid(-1, first - 1))), FCString::Atof(*(Result.Mid(first + 1, second - 1))), FCString::Atof(*(Result.Mid(second + 1, third - 1))));
			}
		}

		SetAlgorithmAxis(X, Y, Z);

		for (int i = third + 1; i < Result.Len(); i++)
		{		
			if (Result[i] != ';')
			{
				TmpString += Result[i];
				continue;
			}
			else
			{
				switch (Count)
				{
					case 1:
						TmpPoint.X = FCString::Atof(*TmpString);
						Count++;
						break;
					case 2:
						TmpPoint.Y = FCString::Atof(*TmpString);
						Count++;
						break;
					case 3:
						TmpPoint.Z = FCString::Atof(*TmpString);
						Count = 1;
						Algorithm->RecordGesture(TmpPoint);
						TmpPoint = FVector::ZeroVector;
						break;
					default:
						Count = 1;
						break;
				}
				TmpString = "";
			}
		}
		AllGestures.Add(*(Algorithm->StopRecordingGesture()));
		LastUsedGestureID++;
		AllGestures.Last().GestureID = LastUsedGestureID;
		//DataLogger->WriteText("Newly added Gesture ID: " + FString::FromInt(AllGestures.Last().GestureID) + " / DotProd: " + FString::SanitizeFloat(AllGestures.Last().DotProductSelf));
		Algorithm->Restart();
		OnNewGestureRecorded.Broadcast();

		//DEBUG
		IndicesToUse.Add(AllGestures.Num() - 1);
	}
}

void UGestureManagerComponent::WriteGesturesToFile(FString _Path, int32 _GestureIndex)
{/*
	FString sName = "Gesture2";
	FString s = _Path + '\\' + sName + ".txt";*/
	
	UDataLogger* Logger = NewObject<UDataLogger>(); 
	Logger->Initilize(_Path  , "Gesture2.txt", true);
	if (AllGestures.IsValidIndex(_GestureIndex)) {
		//void Initilize(FString _path, FString _name, bool _shouldOverride);
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, "I can write Gesture" );

		FGesture Gesture = AllGestures[_GestureIndex];
		for (FVector p : Gesture.RawInput) {
			TArray<FString> Message;
			Message.Add(FString::SanitizeFloat(p.X));
			Message.Add(FString::SanitizeFloat(p.Y));
			Message.Add(FString::SanitizeFloat(p.Z));
			//FString Tempstring = FString::FString(FString::SanitizeFloat(p.X) + FString::SanitizeFloat(p.Y) + FString::SanitizeFloat(p.Z));
			Logger->Write(Message);
			//Logger->WriteText(FString::SanitizeFloat(p.X) + ";" + FString::SanitizeFloat(p.Y) + ";" + FString::SanitizeFloat(p.Z) + ";");

		}

		/*for (int32 i = 0; i < Gesture.InitialRawInputPoint.Size; ++i) {


		}*/
	}
}

FVector UGestureManagerComponent::CalculateRelativeInputPoint()
{
	switch (HandToTrack)
	{
		case  EHandToTrack::HTT_Left:
			return LeftTrackingComponent->CalcRelativeLocation();
			break;

		case  EHandToTrack::HTT_Right:
			return RightTrackingComponent->CalcRelativeLocation();
			break;
		
		default:
			return FVector();
	}
}

void UGestureManagerComponent::SetAlgorithmAxis()
{
	switch (HandToTrack)
	{
	case  EHandToTrack::HTT_Left:
		Algorithm->InitialForwardVector = LeftTrackingComponent->GetForwardVector();
		Algorithm->InitialRightVector = LeftTrackingComponent->GetRightVector();
		Algorithm->InitialUpVector = LeftTrackingComponent->GetUpVector();
		Algorithm2->InitialForwardVector = LeftTrackingComponent->GetForwardVector();
		Algorithm2->InitialRightVector = LeftTrackingComponent->GetRightVector();
		Algorithm2->InitialUpVector = LeftTrackingComponent->GetUpVector();
		break;

	case  EHandToTrack::HTT_Right:
		Algorithm->InitialForwardVector = RightTrackingComponent->GetForwardVector();
		Algorithm->InitialRightVector = RightTrackingComponent->GetRightVector();
		Algorithm->InitialUpVector = RightTrackingComponent->GetUpVector();
		Algorithm2->InitialForwardVector = RightTrackingComponent->GetForwardVector();
		Algorithm2->InitialRightVector = RightTrackingComponent->GetRightVector();
		Algorithm2->InitialUpVector = RightTrackingComponent->GetUpVector();
		break;

	default:
		break;
	}
}

void UGestureManagerComponent::SetAlgorithmAxis(FVector _X, FVector _Y, FVector _Z)
{
	Algorithm->InitialForwardVector = _X;
	Algorithm->InitialRightVector = _Y;
	Algorithm->InitialUpVector = _Z;
	Algorithm2->InitialForwardVector = _X;
	Algorithm2->InitialRightVector = _Y;
	Algorithm2->InitialUpVector = _Z;
}

void UGestureManagerComponent::ValidateGestureFeedback(TArray<FGestureFeedback> _Feedback)
{
	FGestureFeedBackStruct TmpStruct = FGestureFeedBackStruct();
	TmpStruct.Feedbacks = _Feedback;

	if (_Feedback.IsValidIndex(0))
	{
		if (_Feedback.Last().Likelihood > 0.8 && _Feedback.Last().Alignment > 0.95)
		{
			OnGestureDecision.Broadcast(TmpStruct, *(Algorithm->GetActualGesture()));
		}
		else
		{
			OnGestureFailed.Broadcast(TmpStruct, *(Algorithm->GetActualGesture()));
		}
	}
	else
	{
		OnGestureFailed.Broadcast(TmpStruct, *(Algorithm->GetActualGesture()));
	}
}

bool UGestureManagerComponent::CheckForDublicates(FGesture * _GestureToCheck)
{
	TArray<FGestureFeedback> TmpFeedbackArray;
	UCrossProductAlgorithm* TmpAlgorithm = NewObject<UCrossProductAlgorithm>();
	TArray<int32> TmpIndices;
	
	for (FVector& TmpRawInput : _GestureToCheck->RawInput)
	{
		TmpAlgorithm->RecordGesture(TmpRawInput);
	}

	for (int i = 0; i < AllGestures.Num(); i++)
	{
		TmpIndices.Add(i);
	}

	for (FGesture& TmpGesture : AllGestures)
	{
		TmpFeedbackArray = TmpAlgorithm->CalculateGestureFeedback(TmpIndices, FVector::ZeroVector);
	}

	for (FGestureFeedback TmpFeedback : TmpFeedbackArray)
	{
		if (TmpFeedback.Likelihood > 0.90) return false;
	}

	return true;
}

void UGestureManagerComponent::SwitchAlgorithm(EGestureAlgorithm _NewAlgorithmToUse)
{
	AlgorithmToUse = _NewAlgorithmToUse;

	switch (AlgorithmToUse)
	{	
		case EGestureAlgorithm::GA_CP:		
			Algorithm = NewObject<UCrossProductAlgorithm>();
			Algorithm->Initialize(&AllGestures);
			DataLogger->WriteText("Used Algorithm: Cross Product");
			Algorithm2 = NewObject<UMonteCarloAlgorithm>();
			((UMonteCarloAlgorithm*)Algorithm2)->Initialize(&AllGestures, MonteCarloParameters);
			break;	

		case EGestureAlgorithm::GA_MC:
		default:
			Algorithm = NewObject<UMonteCarloAlgorithm>();
			((UMonteCarloAlgorithm*)Algorithm)->Initialize(&AllGestures, MonteCarloParameters);
			DataLogger->WriteText("Used Algorithm: Monte Carlo");
			Algorithm2 = NewObject<UCrossProductAlgorithm>();
			Algorithm2->Initialize(&AllGestures);
			break;
	}
}

EGestureRecognitionState UGestureManagerComponent::GetGestureRecognitionState()
{
	return GestureRecognitionState;
}

TArray<FGesture> UGestureManagerComponent::GetAllGestures()
{
	return AllGestures;
}

void UGestureManagerComponent::StartRecognition(EHandToTrack _HandToTrack)
{	
	if (GestureRecognitionState == EGestureRecognitionState::GRS_Idle)
	{
		HandToTrack = _HandToTrack;
		SetAlgorithmAxis();
		GestureRecognitionState = EGestureRecognitionState::GRS_Compare;
	}
}

void UGestureManagerComponent::StartRecording(EHandToTrack _HandToTrack)
{
	if (GestureRecognitionState == EGestureRecognitionState::GRS_Idle)
	{
		HandToTrack = _HandToTrack;
		SetAlgorithmAxis();
		GestureRecognitionState = EGestureRecognitionState::GRS_Record;
	}
}

void UGestureManagerComponent::StopRecognition()
{
	FGestureFeedBackStruct TmpStruct = FGestureFeedBackStruct();

	if (GestureRecognitionState == EGestureRecognitionState::GRS_Compare)
	{		
		GestureRecognitionState = EGestureRecognitionState::GRS_Idle;

		switch (AlgorithmToUse)
		{
			case EGestureAlgorithm::GA_MC:
				ValidateGestureFeedback(ActualFeedback);
				TmpStruct.Feedbacks = Algorithm2->CalculateGestureFeedback(IndicesToUse, FVector::ZeroVector);
				OnGestureDecision2.Broadcast(TmpStruct);
				ActualFeedback.Empty();
				break;

			case EGestureAlgorithm::GA_CP:
				ValidateGestureFeedback(Algorithm->CalculateGestureFeedback(IndicesToUse, FVector::ZeroVector));
				TmpStruct.Feedbacks = ActualFeedback;
				OnGestureDecision2.Broadcast(TmpStruct);
				ActualFeedback.Empty();
				break;

			default:
				break;
		}
	//for (int i = 0; i < TmpGestureFeedback.Num(); i++)
	//{
	//	DataLogger->WriteText("---------- Feedback[" + FString::FromInt(i) + "] ----------");
	//	DataLogger->WriteText(TmpGestureFeedback[i].ToString());
	//}
		Algorithm->Restart();
		Algorithm2->Restart();
	}
}

void UGestureManagerComponent::StopRecording()
{
	if (GestureRecognitionState == EGestureRecognitionState::GRS_Record)
	{
		GestureRecognitionState = EGestureRecognitionState::GRS_Idle;

		//if (CheckForDublicates(Algorithm->GetActualGesture()))
		//{
			AllGestures.Add(*(Algorithm->StopRecordingGesture()));
			LastUsedGestureID++;
			AllGestures.Last().GestureID = LastUsedGestureID;
			//DataLogger->WriteText("Newly added Gesture ID: " + FString::FromInt(AllGestures.Last().GestureID) + " / DotProd: " + FString::SanitizeFloat(AllGestures.Last().DotProductSelf));
			Algorithm->Restart();
			OnNewGestureRecorded.Broadcast();

			//DEBUG
			IndicesToUse.Add(AllGestures.Num() - 1);
		//}
		//else
		//{
		//	OnGestureRedcordedFailed.Broadcast();
		//}
	}
}

void UGestureManagerComponent::StartSimulation(FVector _Up, FVector _Right, FVector _Forward)
{
	Algorithm->InitialForwardVector = _Forward;
	Algorithm->InitialRightVector = _Right;
	Algorithm->InitialUpVector = _Up;
	Algorithm2->InitialForwardVector = _Forward;
	Algorithm2->InitialRightVector = _Right;
	Algorithm2->InitialUpVector = _Up;
	GestureRecognitionState = EGestureRecognitionState::GRS_Compare;
}

void UGestureManagerComponent::Simulate(FVector _input)
{
	FGestureFeedBackStruct TmpStruct = FGestureFeedBackStruct();

	switch(AlgorithmToUse)
	{
			case EGestureAlgorithm::GA_CP:
				Algorithm->RecordGesture(_input);
				ActualFeedback = Algorithm2->CalculateGestureFeedback(IndicesToUse, _input);
				//TmpStruct.Feedbacks = ActualFeedback;
				break;

			case EGestureAlgorithm::GA_MC:
			default:
				ActualFeedback = Algorithm->CalculateGestureFeedback(IndicesToUse, _input);
				TmpStruct.Feedbacks = ActualFeedback;
				OnGestureUpdate.Broadcast(TmpStruct);
				Algorithm2->RecordGesture(_input);
				break;
	}
}

void UGestureManagerComponent::StopSimulate()
{
	FGestureFeedBackStruct TmpStruct = FGestureFeedBackStruct();

	if (GestureRecognitionState == EGestureRecognitionState::GRS_Compare)
	{
		GestureRecognitionState = EGestureRecognitionState::GRS_Idle;

		switch (AlgorithmToUse)
		{
		case EGestureAlgorithm::GA_MC:
			ValidateGestureFeedback(ActualFeedback);
			TmpStruct.Feedbacks = Algorithm2->CalculateGestureFeedback(IndicesToUse, FVector::ZeroVector);
			OnGestureDecision2.Broadcast(TmpStruct);
			ActualFeedback.Empty();
			break;

		case EGestureAlgorithm::GA_CP:
			ValidateGestureFeedback(Algorithm->CalculateGestureFeedback(IndicesToUse, FVector::ZeroVector));
			TmpStruct.Feedbacks = ActualFeedback;
			OnGestureDecision2.Broadcast(TmpStruct);
			ActualFeedback.Empty();
			break;

		default:
			break;
		}

		Algorithm->Restart();
		Algorithm2->Restart();
	}
}

// Sets default values for this component's properties
UGestureManagerComponent::UGestureManagerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

}

// Called when the game starts
void UGestureManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

// Called every frame
void UGestureManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	FGestureFeedBackStruct TmpStruct = FGestureFeedBackStruct();
	switch (GestureRecognitionState)
	{		
		case EGestureRecognitionState::GRS_Idle:
		default:
			break;

		case EGestureRecognitionState::GRS_Compare:

			switch (AlgorithmToUse)
			{
			case EGestureAlgorithm::GA_CP:
				Algorithm->RecordGesture(CalculateRelativeInputPoint());
				ActualFeedback = Algorithm2->CalculateGestureFeedback(IndicesToUse, CalculateRelativeInputPoint());
				//TmpStruct.Feedbacks = ActualFeedback;
				break;

			case EGestureAlgorithm::GA_MC:
			default:
				ActualFeedback = Algorithm->CalculateGestureFeedback(IndicesToUse, CalculateRelativeInputPoint());
				TmpStruct.Feedbacks = ActualFeedback;
				OnGestureUpdate.Broadcast(TmpStruct);
				Algorithm2->RecordGesture(CalculateRelativeInputPoint());
				break;
			}
			break;

		case EGestureRecognitionState::GRS_Record:
			Algorithm->RecordGesture(CalculateRelativeInputPoint());
			break;
	}
}