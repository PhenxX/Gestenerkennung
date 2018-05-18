#pragma once
#include <limits>
#include "DataLogger.h"
#include "Engine.h"
#include "GestureUtils.generated.h"

UENUM(BlueprintType)
enum class EGestureRecognitionState : uint8
{
	GRS_Idle		UMETA(DisplayName = "Idle"),
	GRS_Compare		UMETA(DisplayName = "Compare"),
	GRS_Record		UMETA(DisplayName = "Record")
};

UENUM(BlueprintType)
enum class EGestureAlgorithm : uint8
{
	GA_MC			UMETA(DisplayName = "Monte_Carlo"),
	GA_CP			UMETA(DisplayName = "Cross_Product")
};

UENUM(BlueprintType)
enum class EHandToTrack : uint8
{
	HTT_Left			UMETA(DisplayName = "Left"),
	HTT_Right			UMETA(DisplayName = "Right")
};

USTRUCT(BlueprintType)
struct FGesture
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
		int32 GestureID;

	UPROPERTY(BlueprintReadOnly)
		FMatrix LocalAxis;

	//The un-normalized input points given by the GestureManager object
	UPROPERTY(BlueprintReadOnly)
		TArray<FVector> RawInput;

	//The un-normalized input points applied on local axis given by the GestureManager object
	UPROPERTY(BlueprintReadOnly)
		TArray<FVector> LocalRawInput;

	//The normalized input points according to Max- and MinRecognizedRange. Calculated by NormalizeInputPoints();
	UPROPERTY(BlueprintReadOnly)
		TArray<FVector> NormalizedInput;

	//The normalized input points according to Max- and MinRecognizedRange with equal space between all vectors. Calculated by SpaceOutPoints();
	UPROPERTY(BlueprintReadOnly)
		TArray<FVector> NormalizedSpacedInput;

	//How many points to compare should be used?
	float AmountSpacedOutPoints;
	FVector MaxRecognizedRange;
	FVector MinRecognizedRange;
	FVector InitialRawInputPoint;

	//CROSS PRODUCT
	float DotProductSelf;


	//MONTE CARLO

	float EstimatedAlignment;
	float EstimatedProbabilities;

private:

	FMatrix TransformationMatrix;
	FMatrix GlobalAxis;

public:

	FORCEINLINE bool FGesture::operator== (const FGesture& Other) const
	{
		return GestureID == Other.GestureID;
	}

	FORCEINLINE bool FGesture::operator!= (const FGesture& Other) const
	{
		return !(GestureID == Other.GestureID);
	}

	friend uint32 GetTypeHash(const FGesture& Other)
	{
		return GetTypeHash(Other.GestureID);
	}

	void SetLocalAxis(FVector _NewXAxis, FVector _NewYAxis, FVector _NewZAxis)
	{
		LocalAxis = FMatrix(_NewXAxis, _NewYAxis, _NewZAxis, FVector::ZeroVector);
		TransformationMatrix = (LocalAxis.Inverse() * GlobalAxis);
	}

	void InitEstimates()
	{
		EstimatedAlignment = 0;
		EstimatedProbabilities = 0;
	}

	void CalculateDotProduct()
	{
		DotProductSelf = 0.f;
		for (int i = 0; i < NormalizedSpacedInput.Num(); ++i)
		{
			DotProductSelf += (NormalizedSpacedInput[i].X * NormalizedSpacedInput[i].X) + (NormalizedSpacedInput[i].Y * NormalizedSpacedInput[i].Y) + (NormalizedSpacedInput[i].Z * NormalizedSpacedInput[i].Z);
		}
	}

	void NormalizeInputPoints(TArray<FVector>* _Array)
	{
		if (_Array->IsValidIndex(0))
		{
			FVector RecognitionDistance = MaxRecognizedRange - MinRecognizedRange;
			float Scale = FMath::Max3(RecognitionDistance.X, RecognitionDistance.Y, RecognitionDistance.Z);
		
			NormalizedInput.Empty();
			NormalizedInput.SetNumUninitialized(_Array->Num());

			for (int i = 0; i < _Array->Num(); ++i)
			{
				NormalizedInput[i] = (*_Array)[i] / Scale;
			}
		}
	}

	float GetStrokeLenght()
	{
		if (NormalizedInput.Num() < 2)
		{
			return 0.f;
		}

		float Length = 0.0f;
		FVector TmpVector;

		for (int i = 0; i < NormalizedInput.Num() - 1; ++i)
		{
			TmpVector = NormalizedInput[i] - NormalizedInput[i+1];
			Length += TmpVector.Size();
		}

		return Length;
	}

	void SpaceOutPoints()
	{
		if (NormalizedInput.Num() < 2)
		{
			return;
		}

		float LengthPastSegment = 0.f; // laenge des current Segment
		float LengthRawPoints = 0.f; // laenge der beiden Raw Punkte
		FVector Point1 = NormalizedInput[0]; // punkt 1 des Segments
		FVector Point2 = NormalizedInput[1]; // Punkt 2 des Segments
		float Length = GetStrokeLenght();
		float LengthPerPoint = Length / AmountSpacedOutPoints;
		NormalizedSpacedInput.Reset();
		NormalizedSpacedInput.Add(Point1); // Add first points

		int i = 1;
		bool ShouldContinue = true;

		while (ShouldContinue) {
			LengthRawPoints = (Point1 - Point2).Size();

			if (LengthRawPoints + LengthPastSegment < LengthPerPoint) { // Abstand ist noch zu klein, gehe einen Punkt weiter
				LengthPastSegment += LengthRawPoints;
				Point1 = Point2;
				++i;
				Point2 = NormalizedInput[i];

			}
			else { // Abstand ist zu gross/richtig, fuege Punkt an der berechneten Stelle hinzu.
				//float amountToMovePoint = (lengthRawPoints + lengthPastSegment) - lengthPerPoint;
				float AmountToMovePoint = LengthPerPoint - LengthPastSegment;
				FVector MoveVector = (Point2 - Point1);
				MoveVector.Normalize();
				NormalizedSpacedInput.Add(Point1 + (MoveVector * AmountToMovePoint));

				Point1 = NormalizedSpacedInput.Last();
				LengthPastSegment = 0;
				LengthRawPoints = 0;
			}
			if (Point2 == NormalizedInput.Last() && (Point1 - Point2).Size() <= LengthPerPoint) {
				ShouldContinue = false;
			}

		}
		// durch float fehler kann es sein das der Letzte Punkt nicht hinzugefügt wurde
		if (NormalizedInput.Num() < AmountSpacedOutPoints)
		{
			NormalizedSpacedInput.Add(NormalizedInput.Last());
		}

		SetGestureOrigin();
	}

	void SetGestureOrigin()
	{
		FVector Origin = FVector::ZeroVector;

		for (FVector tmpVector : NormalizedSpacedInput)
		{
			Origin += tmpVector;
		}

		Origin /= NormalizedSpacedInput.Num();

		for (int i = 0; i < NormalizedSpacedInput.Num(); ++i)
		{
			NormalizedSpacedInput[i] -= Origin;
		}
	}

	void ClampInputPosition(FVector & InputPosition)
	{
		if (MinRecognizedRange.X > InputPosition.X) MinRecognizedRange.X = InputPosition.X;
		if (MinRecognizedRange.Y > InputPosition.Y) MinRecognizedRange.Y = InputPosition.Y;
		if (MinRecognizedRange.Z > InputPosition.Z) MinRecognizedRange.Z = InputPosition.Z;
		if (MaxRecognizedRange.X < InputPosition.X) MaxRecognizedRange.X = InputPosition.X;
		if (MaxRecognizedRange.Y < InputPosition.Y) MaxRecognizedRange.Y = InputPosition.Y;
		if (MaxRecognizedRange.Z < InputPosition.Z) MaxRecognizedRange.Z = InputPosition.Z;
	}

	FVector TransformToLocalCoordianteSystem(FVector _InVector)
	{				
		return FVector(
			(TransformationMatrix.GetColumn(0) * _InVector).X + (TransformationMatrix.GetColumn(0) * _InVector).Y + (TransformationMatrix.GetColumn(0) * _InVector).Z,
			(TransformationMatrix.GetColumn(1) * _InVector).X + (TransformationMatrix.GetColumn(1) * _InVector).Y + (TransformationMatrix.GetColumn(1) * _InVector).Z,
			(TransformationMatrix.GetColumn(2) * _InVector).X + (TransformationMatrix.GetColumn(2) * _InVector).Y + (TransformationMatrix.GetColumn(2) * _InVector).Z
		);
	}

	void ToLocalSpace()
	{
		LocalRawInput.Empty();
		LocalRawInput.SetNumUninitialized(RawInput.Num());

		for (int i = 0; i < RawInput.Num(); ++i)
		{
			LocalRawInput[i] = TransformToLocalCoordianteSystem(RawInput[i]);
		}

		ClampInputPosition(LocalRawInput.Last());
	}

	void AddObservation(FVector InputPosition)
	{
		if (RawInput.Num() == 0)
		{
			InitialRawInputPoint = InputPosition;
		}

		InputPosition -= InitialRawInputPoint;
		RawInput.Add(InputPosition);
		ToLocalSpace();
		NormalizeInputPoints(&LocalRawInput);
	}

	void ResetGesture()
	{
		MaxRecognizedRange = FVector(-INFINITY);
		MinRecognizedRange = FVector(INFINITY);
		NormalizedInput.Empty();
		RawInput.Empty();
		InitialRawInputPoint = FVector::ZeroVector;
		EstimatedAlignment = 0;
		EstimatedProbabilities = 0;
	}

	FGesture()
	{
		GestureID = 0;
		GlobalAxis = FMatrix(
			FVector(1, 0, 0),
			FVector(0, 1, 0),
			FVector(0, 0, 1),
			FVector::ZeroVector
		);
		AmountSpacedOutPoints = 64;
		ResetGesture();
	}
};

USTRUCT(BlueprintType)
struct FGestureFeedback
{
	GENERATED_BODY()

		//Unique ID of the gesture
	UPROPERTY(BlueprintReadOnly)
		int32 GestureID;

	//How likely is the given gesture
	UPROPERTY(BlueprintReadOnly)
		float Likelihood;

	//The progress of time (start to end) of the gesture, clamped between 0f and 1f
	UPROPERTY(BlueprintReadOnly)
		float Alignment;

	FORCEINLINE bool FGestureFeedback::operator== (const FGestureFeedback& Other) const
	{
		return GestureID == Other.GestureID;
	}

	FORCEINLINE bool FGestureFeedback::operator!= (const FGestureFeedback& Other) const
	{
		return !(GestureID == Other.GestureID);
	}

	FORCEINLINE bool FGestureFeedback::operator> (const FGestureFeedback& Other) const
	{
		return Likelihood > Other.Likelihood;
	}

	FORCEINLINE bool FGestureFeedback::operator< (const FGestureFeedback& Other) const
	{
		return Likelihood < Other.Likelihood;
	}

	FORCEINLINE bool FGestureFeedback::operator>= (const FGestureFeedback& Other) const
	{
		return Likelihood >= Other.Likelihood;
	}

	FORCEINLINE bool FGestureFeedback::operator<= (const FGestureFeedback& Other) const
	{
		return Likelihood <= Other.Likelihood;
	}

	FString ToString()
	{
		return "Gesture ID: " + FString::FromInt(GestureID) + "\n Likelihood: " + FString::SanitizeFloat(Likelihood) + "\n Alignment: " + FString::SanitizeFloat(Alignment);
	}

	FGestureFeedback()
	{
		FGestureFeedback(0, 0.0f, 0.0f);
	}

	FGestureFeedback(int32 _GestureID, float _Likelihood, float _Alignment)
	{
		GestureID = _GestureID;
		Likelihood = _Likelihood;
		Alignment = _Alignment;
	}
};