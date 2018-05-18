// Fill out your copyright notice in the Description page of Project Settings.

#include "MyDataReader.h"


bool UMyDataReader::fileExists(const std::string & filename)
{
	struct stat buffer;
	return (stat(filename.c_str(), &buffer) == 0);

}

std::string UMyDataReader::convertToCorrectPath(std::string _path)
{

	std::replace(_path.begin(), _path.end(), '\\', '/');
	return _path;
}

bool UMyDataReader::Initilize(FString _path, FString _name)
{
	path = TCHAR_TO_UTF8(*_path);
	path = convertToCorrectPath(path);
	std::string name = TCHAR_TO_UTF8(*_name);
	if (fileExists(path + "/" + name)) {
		file.open(path + "/" + name);
		return true;
	}
	return false;
}

TArray<FVector> UMyDataReader::ReadData(FString _seperator, int32 _position)
{
	TArray<FVector> returnValue = TArray<FVector>();
	if (file.is_open())
	{
		char line[256];
		bool foundEndOfGesture = false;
		int32 position = 0;
		file.clear();
		file.seekg(0, std::ios::beg);
		while (file.getline(line, 256))
		{
			
			//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Ich bin in der while schleife")); 
			FString stringLine = FString(UTF8_TO_TCHAR(line));
			 {
				if ((stringLine.Compare(_seperator))) {
					if (position == _position) {
						FString x, y, z;
						FString chopedString = stringLine;
						FString leftString;
						FString rightString;
						int32 index = 0;

						//x
						index = chopedString.Find(";");
						leftString = chopedString.LeftChop(chopedString.Len() - index);
						rightString = chopedString.RightChop(index + 1);
						x = leftString;
						chopedString = chopedString.RightChop(index + 1);
						//y
						index = chopedString.Find(";");
						leftString = chopedString.LeftChop(chopedString.Len() - index);
						rightString = chopedString.RightChop(index + 1);
						y = leftString;
						chopedString = chopedString.RightChop(index + 1);
						//z
						index = chopedString.Find(";");
						leftString = chopedString.LeftChop(chopedString.Len() - index);
						rightString = chopedString.RightChop(index + 1);
						z = leftString;
						chopedString = chopedString.RightChop(index + 1);

						FVector v = FVector();
						v.X = FCString::Atof(*x);
						v.Y = FCString::Atof(*y);
						v.Z = FCString::Atof(*z);

						returnValue.Add(v);
						//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, v.ToString() );
					}

					
				}
				else {	
					position++;
					//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("New Gesture"));
					}
			}
		}
		
	}

	return returnValue;
}

void UMyDataReader::Close()
{
	if (file.is_open()) {
		file.close();
	}
}

int32 UMyDataReader::GetWordAmount(FString _word)
{ 
	int32 amount = 0;
	if (file.is_open())
	{
		char line[256];

		while (file.getline(line, 256))
		{

			//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Ich bin in der while schleife")); 
			FString stringLine = FString(UTF8_TO_TCHAR(line));
			{
				if (!(stringLine.Compare(_word))) {
					amount++;

				}
			}
		}
	}
	return amount;
}
