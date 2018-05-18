// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include "Engine.h"
#include "DataLogger.generated.h"

USTRUCT(BlueprintType)
struct FGestureResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
		float LikliehoodCP;
	UPROPERTY(BlueprintReadWrite)
		float LikliehoodMonte;
	UPROPERTY(BlueprintReadWrite)
		float Alingnment;

	FString ToString()
	{
		/*return "Gesture ID: " + FString::FromInt(GestureID) + "\n Likelihood: " + FString::SanitizeFloat(Likelihood) + "\n Alignment: " + FString::SanitizeFloat(Alignment);*/
		return (FString::SanitizeFloat(LikliehoodCP) + ";" + FString::SanitizeFloat(LikliehoodMonte) + ";" + FString::SanitizeFloat(Alingnment)) ;
	}
};
// use for thesis loggin
USTRUCT(BlueprintType)
struct FGestureLogLine

{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
		int32 ID;
	UPROPERTY(BlueprintReadWrite)
		FString Group;

	UPROPERTY(BlueprintReadWrite)
		int32 TryCount;

	UPROPERTY(BlueprintReadWrite)
		int32 AmountRightGestures;

	UPROPERTY(BlueprintReadWrite)
		float TimeNeeded;

	UPROPERTY(BlueprintReadWrite)
		int32 CurrentGestureID;

	UPROPERTY(BlueprintReadWrite)
		FGestureResult Gesture1;

	UPROPERTY(BlueprintReadWrite)
		FGestureResult Gesture2;
	UPROPERTY(BlueprintReadWrite)
		FGestureResult Gesture3;

	UPROPERTY(BlueprintReadWrite)
		FGestureResult Gesture4;

	UPROPERTY(BlueprintReadWrite)
		FGestureResult Gesture5;
	UPROPERTY(BlueprintReadWrite)
		FGestureResult Gesture6;
};
/**
 * 
 */
UCLASS(BlueprintType)
class GESTENERKENNUNG_API UDataLogger : public UObject
{
	GENERATED_BODY()

private:
	std::ofstream file;
	std::string path;
	std::string name;
	bool shouldOverride = false;
	//chek if file Exists
	
	// replaces "\" with "/"
	std::string convertToCorrectPath(std::string _path);
public:
	bool fileExists(const std::string& _filename);
	UFUNCTION(BlueprintCallable)
		void Initilize(FString _path, FString _name, bool _shouldOverride);
	UFUNCTION(BlueprintCallable)
		void Write(TArray<FString> _message);
	UFUNCTION(BlueprintCallable)
		void WriteGestureLine(FGestureLogLine _message);
	UFUNCTION(BlueprintCallable)
		void WriteText(FString _message);
	UFUNCTION(BlueprintCallable)
		void Close();

	
};

