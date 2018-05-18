// Fill out your copyright notice in the Description page of Project Settings.

#include "MonteCarloAlgorithm.h"

void UMonteCarloAlgorithm::UpdatePrior(FParticle* _Particle)
{
	float GestureLength = GetGestureLength(_Particle->GestureID);

	if (GestureLength == 0)
	{
		return;
	}

	_Particle->Progress += GetRandomNormal() * MCAlgorithmParameters.AlignmentVariance + _Particle->Dynamic.X / GestureLength;

	_Particle->Dynamic.X += GetRandomNormal() * MCAlgorithmParameters.DynamicsVariance.X + _Particle->Dynamic.Y / GestureLength;
	_Particle->Dynamic.Y += GetRandomNormal() * MCAlgorithmParameters.DynamicsVariance.X;

	_Particle->Prior = _Particle->Posterior;
}

void UMonteCarloAlgorithm::UpdateLikelihood(FVector _InputPosition, FParticle* _Particle, int32 _ParticleIndex)
{
	FVector InputPosition = _InputPosition;

	if (_Particle->Progress < 0.f)
	{
		_Particle->Progress = fabs(_Particle->Progress);
	}
	else if (_Particle->Progress > 1.0)
	{
		_Particle->Progress = fabs(2.0 - _Particle->Progress); // re-spread at the end
	}

	FGesture* GestureTemplate = FindGestureByID(_Particle->GestureID);

	if (GestureTemplate == nullptr)
	{
		return;
	}

	int32 Frameindex = FMath::Min((GetGestureLength(GestureTemplate->GestureID) - 1), (int32)(floor(_Particle->Progress * GetGestureLength(GestureTemplate->GestureID))));

	FVector VectorReference = GestureTemplate->LocalRawInput[Frameindex];

	float Dist = EuclideanDistance(VectorReference, InputPosition);

	// Gaussian or Student's distribution
	if (MCAlgorithmParameters.Distribution == 0.0f) {    
		_Particle->Likelihood = exp(-Dist * 1 / (MCAlgorithmParameters.Tolerance * MCAlgorithmParameters.Tolerance));
	}
	else {           
		_Particle->Likelihood = pow(Dist / MCAlgorithmParameters.Distribution + 1, -MCAlgorithmParameters.Distribution / 3 - 1);
	}
}

void UMonteCarloAlgorithm::UpdatePosterior(FParticle* _Particle)
{
	_Particle->Posterior = _Particle->Prior * _Particle->Likelihood;
}

int32 UMonteCarloAlgorithm::GetGestureLength(int32 _GestureID)
{
	for (FGesture* TmpGesturePointer : GesturesToObserv)
	{
		if (TmpGesturePointer->GestureID == _GestureID) return TmpGesturePointer->RawInput.Num();
	}

	return 0;
}

void UMonteCarloAlgorithm::InitializePrior()
{
	for (int i = 0; i < MCAlgorithmParameters.NumberParticles; ++i)
	{
		if (!Particles.IsValidIndex(i))
		{
			continue;
		}

		FParticle* TmpParticle = &Particles[i];

		TmpParticle->Progress = (GetRandomUniform() - 0.5) * MCAlgorithmParameters.AlignmentSpreadingRange + MCAlgorithmParameters.AlignmentSpreadingCenter; // spread phase

		TmpParticle->Dynamic.X = (GetRandomUniform() - 0.5) * MCAlgorithmParameters.DynamicsSpreadingRange + MCAlgorithmParameters.DynamicsSpreadingCenter; // spread speed
		TmpParticle->Dynamic.Y = (GetRandomUniform() - 0.5) * MCAlgorithmParameters.DynamicsSpreadingRange; // spread acceleration

		TmpParticle->Prior = 1.0 / (float)MCAlgorithmParameters.NumberParticles;
		TmpParticle->Posterior = TmpParticle->Prior;
		TmpParticle->GestureID = GetGestureIDByParticleIndex(i);
	}
}

int32 UMonteCarloAlgorithm::GetGestureIDByParticleIndex(int32 _ParticleIndex)
{
	if (GesturesToObserv.IsValidIndex(0))
	{
		int32 GestureIndexToBind = _ParticleIndex % GesturesToObserv.Num();
		int32 i = 0;

		for (int j = 0; j < GesturesToObserv.Num(); ++j)
		{
			if (i == GestureIndexToBind) return GesturesToObserv[j]->GestureID;
			++i;
		}
	}

	return 1;
}

FGesture * UMonteCarloAlgorithm::FindGestureByID(int32 _GestureID)
{
	if (GesturesToObserv.IsValidIndex(0))
	{
		for (FGesture* TmpGesture : GesturesToObserv)
		{
			if (_GestureID == TmpGesture->GestureID) return TmpGesture;
		}
	}

	return nullptr;
}

void UMonteCarloAlgorithm::Resample()
{
	TArray<float> Dist;
	TArray<int>	OldGestureID;
	TArray<float> OldProgression;
	TArray<FVector>	OldDynamic;

	Dist.SetNumUninitialized(MCAlgorithmParameters.NumberParticles);
	OldGestureID.SetNumUninitialized(MCAlgorithmParameters.NumberParticles);
	OldProgression.SetNumUninitialized(MCAlgorithmParameters.NumberParticles);
	OldDynamic.SetNumUninitialized(MCAlgorithmParameters.NumberParticles);

	// Save old data
	for (int i = 0; i < MCAlgorithmParameters.NumberParticles; ++i)
	{
		if (!Particles.IsValidIndex(i))
		{
			continue;
		}

		FParticle* TmpParticle = &Particles[i];

		OldGestureID[i] = TmpParticle->GestureID;
		OldProgression[i] = TmpParticle->Progress;
		OldDynamic[i] = TmpParticle->Dynamic;
	}

	// Calculate distance 
	Dist[0] = 0;

	for (int i = 1; i < MCAlgorithmParameters.NumberParticles; ++i)
	{
		if (!Particles.IsValidIndex(i))
		{
			continue;
		}

		FParticle* TmpParticle = &Particles[i];

		Dist[i] = Dist[i - 1] + TmpParticle->Posterior;
	}

	float u0 = (GetRandomUniform() - 0.5) / MCAlgorithmParameters.NumberParticles;
	int i = 0;

	for (int ParticleIndex = 0; ParticleIndex < MCAlgorithmParameters.NumberParticles; ++ParticleIndex)
	{
		if (!Particles.IsValidIndex(ParticleIndex))
		{
			continue;
		}

		FParticle* TmpParticle = &Particles[ParticleIndex];

		float uj = u0 + (ParticleIndex + 0.) / MCAlgorithmParameters.NumberParticles;

		while (uj > Dist[i] && i < MCAlgorithmParameters.NumberParticles - 1)
		{
			++i;
		}

		TmpParticle->GestureID = OldGestureID[i];
		TmpParticle->Progress = OldProgression[i];
		TmpParticle->Dynamic = OldDynamic[i];

		// update posterior (particles' weights)
		TmpParticle->Posterior = 1.0 / (float)MCAlgorithmParameters.NumberParticles;
	}
}

TArray<FGestureFeedback> UMonteCarloAlgorithm::Estimate()
{
	TArray<FGestureFeedback> Feedback = TArray<FGestureFeedback>();

	for (FGesture* TmpGesture : GesturesToObserv)
	{
		TmpGesture->InitEstimates();
	}

	// compute the estimated features and likelihoods
	for (int ParticleIndex = 0; ParticleIndex <  MCAlgorithmParameters.NumberParticles; ++ParticleIndex)
	{
		if (!Particles.IsValidIndex(ParticleIndex))
		{
			continue;
		}

		FParticle* TmpParticle = &Particles[ParticleIndex];

		FGesture* TmpGesture = FindGestureByID(TmpParticle->GestureID);

		if (TmpGesture == nullptr)
		{
			continue;
		}

		TmpGesture->EstimatedAlignment += TmpParticle->Progress * TmpParticle->Posterior;

		if (!isnan(TmpParticle->Posterior))
		{
			TmpGesture->EstimatedProbabilities += TmpParticle->Posterior;
		}
	}

	for (FGesture* TmpGesture : GesturesToObserv)
	{
		Feedback.Add(FGestureFeedback(TmpGesture->GestureID, TmpGesture->EstimatedProbabilities, TmpGesture->EstimatedAlignment));
	}

	return Feedback;
}

UMonteCarloAlgorithm::UMonteCarloAlgorithm()
{
	Particles = TArray<FParticle>();	
}

UMonteCarloAlgorithm::~UMonteCarloAlgorithm()
{
	delete RandomNormalDistribution;
	delete RandomUniformDistribution;
}

float UMonteCarloAlgorithm::GetRandomUniform()
{
	return (*RandomUniformDistribution)(RandomEngine);
}

float UMonteCarloAlgorithm::GetRandomNormal()
{
	return (*RandomNormalDistribution)(MersenneTwister);
}

void UMonteCarloAlgorithm::Initialize(TArray<FGesture>* _AllGestures)
{
	Super::Initialize(_AllGestures);
}

void UMonteCarloAlgorithm::Initialize(TArray<FGesture>* _AllGestures, FGREngineParameters _MCAlgorithmParameter)
{	
	MersenneTwister = std::mt19937(RandomDevice());
	RandomNormalDistribution = new std::normal_distribution<float>(0.0, 1.0);
	RandomEngine = std::default_random_engine(RandomDevice());
	RandomUniformDistribution = new std::uniform_real_distribution<float>(0.0, 1.0);
	MCAlgorithmParameters = _MCAlgorithmParameter;
	Initialize(_AllGestures);
}

TArray<FGestureFeedback> UMonteCarloAlgorithm::CalculateGestureFeedback(TArray<int32> _GestureIndices, FVector _InputPosition)
{	
	GesturesToObserv.Empty();

	for (int32 GestureIndex : _GestureIndices)
	{
		GesturesToObserv.Add(&((*AllGestures)[GestureIndex]));
	}
	
	if (&ActualGesture == nullptr)
	{
		ActualGesture = FGesture();
		ActualGesture.SetLocalAxis(InitialRightVector, -InitialForwardVector, InitialUpVector);
	}

	ActualGesture.SetLocalAxis(InitialRightVector, -InitialForwardVector, InitialUpVector);
	ActualGesture.AddObservation(_InputPosition);

	FVector TmpInputPoint = ActualGesture.LocalRawInput.Last();

	float SumWeights = 0.0f;

	if (!Particles.IsValidIndex(0))
	{
		Particles.SetNumUninitialized(MCAlgorithmParameters.NumberParticles);
		InitializePrior();
	}
		
	for (int i = 0; i < MCAlgorithmParameters.NumberParticles; ++i)
	{
		if (!Particles.IsValidIndex(i)) continue;

		FParticle* TempParticle = &Particles[i];

		for (int j = 0; j < MCAlgorithmParameters.PredictionSteps; ++j)
		{
			UpdatePrior(TempParticle);
			UpdateLikelihood(TmpInputPoint, TempParticle, i);
			UpdatePosterior(TempParticle);
		}

		SumWeights += TempParticle->Posterior;
	}

	float WeightedDotProduct = 0.f;

	for (int i = 0; i < MCAlgorithmParameters.NumberParticles; ++i)
	{
		if (!Particles.IsValidIndex(i)) continue;

		FParticle* TmpParticle = &Particles[i];

		TmpParticle->Posterior /= SumWeights;
		WeightedDotProduct += TmpParticle->Posterior * TmpParticle->Posterior;
	}

	if ((1. / WeightedDotProduct) < MCAlgorithmParameters.ResamplingThreshold)
	{
		Resample();
	}

	TArray<FGestureFeedback> Feedback = Estimate();

	Feedback.Sort();

	return Feedback;
}

void UMonteCarloAlgorithm::RecordGesture(FVector _InputPosition)
{
	Super::RecordGesture(_InputPosition);
}

FGesture * UMonteCarloAlgorithm::StopRecordingGesture()
{
	return Super::StopRecordingGesture();
}

void UMonteCarloAlgorithm::Restart()
{
	Particles.SetNumUninitialized(MCAlgorithmParameters.NumberParticles);
	InitializePrior();
	Super::Restart();
}

float UMonteCarloAlgorithm::EuclideanDistance(FVector _X, FVector _Y)
{
	float Distance = 0.f;

	for (int i = 0; i <= 2; ++i)
	{
		Distance += pow((_X[i] - _Y[i]), 2);
	}

	return Distance;
}
