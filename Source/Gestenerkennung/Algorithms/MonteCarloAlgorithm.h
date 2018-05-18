// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Algorithms/GestureAlgorithm.h"
#include <random>
#include "MonteCarloAlgorithm.generated.h"

/**
 *
 */

USTRUCT()
struct FGREngineParameters
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly)
		float Tolerance;

	UPROPERTY(EditDefaultsOnly)
		float Distribution;

	UPROPERTY(EditDefaultsOnly)
		int32 NumberParticles;

	UPROPERTY(EditDefaultsOnly)
		int32 ResamplingThreshold;

	UPROPERTY(EditDefaultsOnly)
		float AlignmentVariance;

	/*
	* Change variance of adaptation in dynamics
	* @details if dynamics adaptation variance is high the method will adapt faster to
	* fast changes in dynamics. Dynamics is 2-dimensional: the first dimension is the speed
	* The second dimension is the acceleration.
	*
	* Typically the variance is the average amount the speed or acceleration can change from
	* one sample to another. As an example, if the relative estimated speed can change from 1.1 to 1.2
	* from one sample to another, the variance should allow a change of 0.1 in speed. So the variance
	* should be set to 0.1*0.1 = 0.01
	*/
	UPROPERTY(EditDefaultsOnly)
		FVector DynamicsVariance;

	UPROPERTY(EditDefaultsOnly)
		float AlignmentSpreadingCenter;

	UPROPERTY(EditDefaultsOnly)
		float AlignmentSpreadingRange;

	UPROPERTY(EditDefaultsOnly)
		float DynamicsSpreadingCenter;

	UPROPERTY(EditDefaultsOnly)
		float DynamicsSpreadingRange;

	UPROPERTY(EditDefaultsOnly)
		int32 PredictionSteps;

	FGREngineParameters() : Tolerance(10.f / 3.f), Distribution(0.0f), NumberParticles(1000), ResamplingThreshold(250), AlignmentVariance(sqrt(0.000001f)), DynamicsVariance(FVector(sqrt(0.01f))), AlignmentSpreadingCenter(0.0), AlignmentSpreadingRange(0.2), DynamicsSpreadingCenter(1.0), DynamicsSpreadingRange(0.3), PredictionSteps(1) {}
};

USTRUCT(BlueprintType)
struct FParticle
{
	GENERATED_BODY()

	int32 GestureID;

	// Progress ([0,1] clamped value of time) of the Particle, 0 equals the begin and 1 the end of the given gesture
	float Progress;

	// Instantaneous estimation of the dynamic parameter

	FVector Dynamic;

	float Likelihood;

	float Prior;

	float Posterior;

	FParticle() : GestureID(0), Progress(0.f), Likelihood(0.f), Prior(0.f), Posterior(0.f), Dynamic(FVector::ZeroVector) {}
};

UCLASS()
class GESTENERKENNUNG_API UMonteCarloAlgorithm : public UGestureAlgorithm
{
	GENERATED_BODY()

private:

	FGREngineParameters MCAlgorithmParameters;
	TArray<FParticle> Particles;

	std::random_device RandomDevice;
	std::mt19937 MersenneTwister;
	std::normal_distribution<float>* RandomNormalDistribution;
	std::default_random_engine RandomEngine;
	std::uniform_real_distribution<float>* RandomUniformDistribution;

	void UpdatePrior(FParticle* _Particle);

	void UpdateLikelihood(FVector _InputPosition, FParticle* _Particle, int32 _ParticleIndex);

	void UpdatePosterior(FParticle* _Particle);

	int32 GetGestureLength(int32 _GestureID);

	void InitializePrior();

	int32 GetGestureIDByParticleIndex(int32 _ParticleIndex);

	FGesture* FindGestureByID(int32 _GestureID);

	void Resample();

	TArray<FGestureFeedback> Estimate();

	float EuclideanDistance(FVector _X, FVector _Y);

public:

	UMonteCarloAlgorithm();

	~UMonteCarloAlgorithm();

	float GetRandomUniform();

	float GetRandomNormal();

	virtual void Initialize(TArray<FGesture>* _AllGestures) override;

	virtual void Initialize(TArray<FGesture>* _AllGestures, FGREngineParameters _MCAlgorithmParameter);

	virtual TArray<FGestureFeedback> CalculateGestureFeedback(TArray<int32> _GestureIndices, FVector _InputPosition) override;

	virtual void RecordGesture(FVector _InputPosition) override;

	virtual FGesture* StopRecordingGesture() override;

	//Reset inner states, should be called before start analyzing a new gesture
	virtual void Restart() override;
};
