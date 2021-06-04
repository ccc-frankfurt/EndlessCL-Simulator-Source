// Fill out your copyright notice in the Description page of Project Settings.


#include "RandomnessManager.h"
#include <Engine/World.h>
#include "EndlessRunnerGameMode.h"


// Sets default values
ARandomnessManager::ARandomnessManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void ARandomnessManager::BeginPlay()
{
	Super::BeginPlay();

    // Register to GameMode
    RegisterToGameMode();

    // Set Seed
    RandomEngine.seed(Seed);

    REngine.Initialize(Seed);
    float test = REngine.FRandRange(0.f, 1.f);
    UE_LOG(LogTemp,Error,TEXT("Test: %f"), test);
}

// Called every frame
void ARandomnessManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ARandomnessManager::SetCategoricalSamplingWeights(TMap<FString, float> WeightMap){
    CategoricalSamplingWeights = WeightMap;
    return;
}

float ARandomnessManager::GetFloat(){
    //return REngine.FRandRange(0.f, 1.f);
    std::uniform_real_distribution<float> distribution(0.f,1.f);
    return distribution(RandomEngine);
}

float ARandomnessManager::GetFloatInRange(float Min, float Max){
    //return REngine.FRandRange(Min,Max);
    return (GetFloat() * (Max-Min)) + Min;
}

int32 ARandomnessManager::GetIntInRange(int32 Min, int32 Max){
    //return REngine.RandRange(Min,Max);
    UE_LOG(LogWindows, Warning, TEXT("Entering: GetIntInRange"));
    std::uniform_int_distribution<int32> distribution(Min, Max);
    int32 result = distribution(RandomEngine);

    
    return result;
}

float ARandomnessManager::SampleNormalDistributaion(float Mean, float Stddev){
    std::normal_distribution<float> distribution(Mean, Stddev);
    return distribution(RandomEngine);
}

int32 ARandomnessManager::SampleCategoricalDistribution(TArray<float> SampleWeights){
    // Convert TArray to intermediated vector representation
    std::vector<float> vec(SampleWeights.Num());
    for(size_t i=0;i<SampleWeights.Num();i++){
        vec[i] = SampleWeights[i];
    }
    // Setup discrete distribution
    std::discrete_distribution<int> categoricalDistribution(vec.begin(), vec.end());
    // Sample
    int result = categoricalDistribution(RandomEngine);
    return result;
}

bool ARandomnessManager::CheckProbability(float ProbThreshold){
    if(GetFloat() > ProbThreshold){
        return true;
    }
    return false;
}

/*
 * Resetting the RandomEngine by giving the same seed again
 */
void ARandomnessManager::ResetRandomEngine(){
    RandomEngine.seed(Seed);
}

void ARandomnessManager::ResetRandomEngine(int NewSeed){
    RandomEngine.seed(NewSeed);
}

void ARandomnessManager::RegisterToGameMode(){
    if(GetWorld()){
        if(GetWorld()->GetAuthGameMode()){
            AEndlessRunnerGameMode* gameModeCallback = (AEndlessRunnerGameMode*)GetWorld()->GetAuthGameMode();
            gameModeCallback->RandomnessManager = this;
            UE_LOG(LogTemp, Warning, TEXT("RandomnessManager: Registered to GameMode"));
        }
    }
}