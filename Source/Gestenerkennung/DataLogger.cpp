// Fill out your copyright notice in the Description page of Project Settings.

#include "DataLogger.h"

bool UDataLogger::fileExists(const std::string & filename)
{
	struct stat buffer;
	return (stat(filename.c_str(), &buffer) == 0);



	/*struct stat buf;
	if (stat(filename.c_str(), &buf) != -1)
	{
		return true;
	}
	return false;*/
}

std::string UDataLogger::convertToCorrectPath(std::string _path)
{
	
	std::replace(_path.begin(), _path.end(), '\\', '/');
	return _path;
}

void UDataLogger::Initilize(FString _path, FString _name, bool _shouldOverride)
{
	shouldOverride = _shouldOverride;
	std::string sPath = TCHAR_TO_UTF8(*_path);
	sPath = convertToCorrectPath(sPath);
	std::string sName = TCHAR_TO_UTF8(*_name);
	//if(fileExists(sPath + "/" + sName))
	if (!shouldOverride) {
		while (fileExists(sPath + "/" + sName)) {

			/*if (GEngine) {
				GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("File Already exists"));
			}*/
			// find end of name:
			std::string::iterator si1;
			si1 = sName.begin();

			for (si1 = sName.begin(); si1 < sName.end(); si1++) {
				//the string iterator holds the address of a particular character within the string 
				//so we need to deref the iterators value (i.e. it holds a memory address) to get the character
				if (*si1 == '.') {
					sName.insert(si1, '1');
					si1 = sName.end();
				}
			}
			si1 = sName.begin();
			/*if (GEngine) {
				FString temp(sName.c_str());
				GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString(temp));
			}*/
		}
	}
	/*FString temp((sPath + "/" + sName).c_str());
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString(temp));*/
	if (!fileExists(sPath + "/" + sName ))
	{
		file.open(sPath + "/" + sName);
	}
	else if (shouldOverride)
	{
		file.open(sPath + "/" + sName);

	}
	path = sPath;
	name = sName;
}

void UDataLogger::Write(TArray<FString> _message)
{
	if (file.is_open()) {
		for (int32 i = 0;i < _message.Num(); i++) {
			
			file << TCHAR_TO_UTF8(*_message[i]) << ";";
		}
		file << std::endl;
	}
}

void UDataLogger::WriteGestureLine(FGestureLogLine _message)
{
	if (file.is_open()) {
		 
			file << TCHAR_TO_UTF8(*(FString::SanitizeFloat(_message.ID))) << ";";
			file << TCHAR_TO_UTF8(*_message.Group) << ";";
			file << TCHAR_TO_UTF8(*(FString::SanitizeFloat(_message.TryCount))) << ";";
			file << TCHAR_TO_UTF8(*(FString::SanitizeFloat(_message.AmountRightGestures))) << ";";
			file << TCHAR_TO_UTF8(*(FString::SanitizeFloat(_message.TimeNeeded))) << ";";
			file << TCHAR_TO_UTF8(*(FString::SanitizeFloat(_message.CurrentGestureID))) << ";";
			file << TCHAR_TO_UTF8(*(_message.Gesture1.ToString())) << ";";
			file << TCHAR_TO_UTF8(*(_message.Gesture2.ToString())) << ";";
			file << TCHAR_TO_UTF8(*(_message.Gesture3.ToString())) << ";";
			file << TCHAR_TO_UTF8(*(_message.Gesture4.ToString())) << ";";
			file << TCHAR_TO_UTF8(*(_message.Gesture5.ToString())) << ";";
			file << TCHAR_TO_UTF8(*(_message.Gesture6.ToString())) << ";";

		file << std::endl;
	}
}

void UDataLogger::WriteText(FString _message)
{
	if (file.is_open()) {
		

			file << TCHAR_TO_UTF8(*_message) ;
		
		file << std::endl;
	}
}

void UDataLogger::Close()
{
	if (file.is_open()) {
		file.close();
	}
}
