// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include "Engine.h"
#include "MyDataReader.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class GESTENERKENNUNG_API UMyDataReader : public UObject
{
	GENERATED_BODY()
	
private:
	std::ifstream file;
	std::string path;
	std::string name;
	std::string convertToCorrectPath(std::string _path);
	bool fileExists(const std::string & filename);
public:
	UFUNCTION(BlueprintCallable)
		bool Initilize(FString _path, FString _name);
	//position is the index of the data if there are more than one
	UFUNCTION(BlueprintCallable)
		TArray<FVector> ReadData (FString _seperator, int32 _position);
	UFUNCTION(BlueprintCallable)
		void Close();
	UFUNCTION(BlueprintCallable)
		int32 GetWordAmount(FString _word);
};
