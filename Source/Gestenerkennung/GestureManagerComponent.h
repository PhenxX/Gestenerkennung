// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GestureUtils.h"
#include "Algorithms/GestureAlgorithm.h"
#include "DataLogger.h"
#include "Components/ActorComponent.h"
#include "Algorithms/CrossProductAlgorithm.h"
#include "GestureTrackingComponent.h"
#include <EngineGlobals.h>
#include <Runtime/Engine/Classes/Engine/Engine.h>
#include "Algorithms/MonteCarloAlgorithm.h"
#include "GestureManagerComponent.generated.h"

USTRUCT(BlueprintType)
struct FGestureFeedBackStruct
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
		TArray<FGestureFeedback> Feedbacks;

	FGestureFeedBackStruct() {}
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGestureUpdate, FGestureFeedBackStruct, ActualGestureData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FGestureDecision, FGestureFeedBackStruct, ActualGestureData,  FGesture, ActualGesture);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGestureRecorded);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FGestureFailed, FGestureFeedBackStruct, ActualGestureData, FGesture, ActualGesture);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGestureRecordedFailed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGesturesCleared);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGestureDecision2, FGestureFeedBackStruct, ActualGestureData);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GESTENERKENNUNG_API UGestureManagerComponent : public UActorComponent
{
	GENERATED_BODY()

private:

	//COMPONENTS
	UPROPERTY()
		UGestureAlgorithm* Algorithm;
	UPROPERTY()
		UGestureAlgorithm* Algorithm2;

	UPROPERTY()
		UDataLogger* DataLogger;

	UPROPERTY()
		UGestureTrackingComponent* LeftTrackingComponent;
	
	UPROPERTY()
		UGestureTrackingComponent* RightTrackingComponent;

	UPROPERTY(EditDefaultsOnly)
		FGREngineParameters MonteCarloParameters;

	EHandToTrack HandToTrack;
	EGestureRecognitionState GestureRecognitionState;
	TArray<FGesture> AllGestures;
	TArray<FGestureFeedback> ActualFeedback;
	EGestureAlgorithm AlgorithmToUse;

	//DEBUG
	int32 LastUsedGestureID = 0;
	TArray<int32> IndicesToUse;

	//Load Gestures from File
	void LoadGesturesFromFile(FString _Path);

	

	FVector CalculateRelativeInputPoint();

	void SetAlgorithmAxis();
	void SetAlgorithmAxis(FVector _X, FVector _Y, FVector _Z);

	void ValidateGestureFeedback(TArray<FGestureFeedback> _Feedback);

	bool CheckForDublicates(FGesture* _GestureToCheck);

public:

	UFUNCTION(BlueprintPure, Category = "Gesture Recognition")
		UDataLogger* GetDataLogger();

	//Initialise default variables and components, called in constructor
	UFUNCTION(BlueprintCallable, Category = "Gesture Recognition")
		void Initialise(UGestureTrackingComponent* _LeftTrackingComponent, UGestureTrackingComponent* _RightTrackingComponent, FString _Path);

	UFUNCTION(BlueprintCallable, Category = "Gesture Recognition")
		FGesture GetGestureByID(int32 _GestureIDToFind);

	UFUNCTION(BlueprintCallable, Category = "Gesture Recognition")
		void ClearGestures();

	UFUNCTION(BlueprintCallable, Category = "Gesture Recognition")
		void SwitchAlgorithm(EGestureAlgorithm _NewAlgorithmToUse);

	UPROPERTY(BlueprintAssignable, Category = "Gesture Recognition")
		FGestureUpdate OnGestureUpdate;

	UPROPERTY(BlueprintAssignable, Category = "Gesture Recognition")
		FGestureDecision OnGestureDecision;

	UPROPERTY(BlueprintAssignable, Category = "Gesture Recognition")
		FGestureDecision2 OnGestureDecision2;

	UPROPERTY(BlueprintAssignable, Category = "Gesture Recognition")
		FGestureRecorded OnNewGestureRecorded;

	UPROPERTY(BlueprintAssignable, Category = "Gesture Recognition")
		FGestureFailed OnGestureFailed;

	UPROPERTY(BlueprintAssignable, Category = "Gesture Recognition")
		FGestureRecordedFailed OnGestureRedcordedFailed;

	UPROPERTY(BlueprintAssignable, Category = "Gesture Recognition")
		FGesturesCleared OnGesturesCleared;

	UFUNCTION(BlueprintPure, Category = "Gesture Recognition")
		EGestureRecognitionState GetGestureRecognitionState();

	UFUNCTION(BlueprintPure, Category = "Gesture Recognition")
		TArray<FGesture> GetAllGestures();

	UFUNCTION(BlueprintCallable, Category = "Gesture Recognition")
		void StartRecognition(EHandToTrack _HandToTrack);

	UFUNCTION(BlueprintCallable, Category = "Gesture Recognition")
		void StartRecording(EHandToTrack _HandToTrack);

	UFUNCTION(BlueprintCallable, Category = "Gesture Recognition")
		void StopRecognition();

	UFUNCTION(BlueprintCallable, Category = "Gesture Recognition")
		void StopRecording();

	UFUNCTION(BlueprintCallable, Category = "Gesture Recognition")
		void StartSimulation(FVector _Up, FVector _Right, FVector _Forward);
	UFUNCTION(BlueprintCallable, Category = "Gesture Recognition")
		void Simulate(FVector _input);
	UFUNCTION(BlueprintCallable, Category = "Gesture Recognition")
		void StopSimulate();
	UFUNCTION(BlueprintCallable, Category = "Gesture Recognition")
		void WriteGesturesToFile(FString _Path, int32 _GestureIndex);
	UGestureManagerComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
	
};
