// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <stdlib.h>
#include <random>
#include <functional>
#include "Math/RandomStream.h"
#include "RandomnessManager.generated.h"

UCLASS()
class ENDLESSRUNNER_API ARandomnessManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARandomnessManager();

    UPROPERTY(EditAnywhere, Category="Seed")
    int32 Seed = 42;

    // Mapping a path (to Asset instance / to directory of assets) to a sampling probability (0.0, 1.0)
    UPROPERTY(EditAnywhere, Category="Categorical Sampling Weights")
    TMap<FString, float> CategoricalSamplingWeights;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

    void RegisterToGameMode();

    std::default_random_engine RandomEngine;

    FRandomStream REngine;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

    void SetCategoricalSamplingWeights(TMap<FString, float> WeightMap);

    float GetFloat();
    float GetFloatInRange(float Min, float Max);

    int32 GetIntInRange(int32 Min, int32 Max);

    float SampleNormalDistributaion(float Mean, float Stddev);

    int32 SampleCategoricalDistribution(TArray<float> SampleWeights);

    bool CheckProbability(float ProbThreshold);

    void ResetRandomEngine();
    void ResetRandomEngine(int NewSeed);
};
