// Fill out your copyright notice in the Description page of Project Settings.

#include "ComplexityManager.h"
#include "Materials/MaterialInstanceDynamic.h"
#include <Components/StaticMeshComponent.h>
#include "RandomnessManager.h"
#include "EnvironmentManager.h"
#include "TileManager.h"
#include "EndlessRunnerGameMode.h"

#include "Tiles/BaseTile.h"


// Sets default values
AComplexityManager::AComplexityManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AComplexityManager::BeginPlay()
{
	Super::BeginPlay();

    // Register to GameMode
    RegisterToGameMode();

    if(GetWorld()){ // nullptr check for World
        // Set Callback to EnvironmentManager
        EnvironmentManagerCallback = ((AEndlessRunnerGameMode*)GetWorld()->GetAuthGameMode())->FindEnvironmentManager();
        // Set Callback to RandomnessManager
        RandomnessManagerCallback = ((AEndlessRunnerGameMode*)GetWorld()->GetAuthGameMode())->FindRandomnessManager();
        // Set Callback to TileManager
        TileManagerCallback = ((AEndlessRunnerGameMode*)GetWorld()->GetAuthGameMode())->FindTileManager();
    }
    
}

// Called every frame
void AComplexityManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AComplexityManager::LoadSequenceModule(FSequence SequenceSetting){
    // Copy the SequenceManager's ActiveSequence settings
    ActiveSequence = SequenceSetting;
    LoadSubSequence();
}

void AComplexityManager::LoadSubSequence(){
    // Load number of additional tiles to pass for the next setting
    NextSubSequenceIncrease += ActiveSequence.SubSequenceSettings[SubSequenceIndex].NumberOfTiles;

    // Load Scene Settings
    ObjectPermutation = ActiveSequence.SubSequenceSettings[SubSequenceIndex].ObjectPresence;
    MaterialActivationPermutation = ActiveSequence.SubSequenceSettings[SubSequenceIndex].MaterialSettings;
    EnvironmentSettings = ActiveSequence.SubSequenceSettings[SubSequenceIndex].EnvironmentSettings;

    /* Do not propagate, because this needs to be triggered by the tile the camera is on
    // Propagate to EnvironmentManager
    // this is here, because the ComplexityManager handles SubSequences
    if(EnvironmentManagerCallback){
        EnvironmentManagerCallback->SetNewEnvironment(EnvironmentSettings, true);
    } else{
        UE_LOG(LogTemp, Fatal, TEXT("ComplexityManager: LoadSubSequence: No EnvironmentManagerCallback.."));
    }
    */
    // Propagate to RandomnessManager
    if(RandomnessManagerCallback){
        RandomnessManagerCallback->SetCategoricalSamplingWeights(ActiveSequence.SubSequenceSettings[SubSequenceIndex].CategoricalSamplingWeights);
    } else{
        UE_LOG(LogTemp, Fatal, TEXT("ComplexityManager: LoadSubSequence: No RandomnessManagerCallback.."));
    }
    // Propagate to TileManager
    if(TileManagerCallback){
        //TileManagerCallback->TileObjectSamplingNumbers = ActiveSequence.SubSequenceSettings[SubSequenceIndex].ObjectSamplingNumbers;
        TileManagerCallback->LoadSubSequenceModule(ActiveSequence.SubSequenceSettings[SubSequenceIndex]);
    } else {
        UE_LOG(LogTemp, Fatal, TEXT("ComplexityManager: LoadSubSequence: No TileManagerCallback.."));
    }
}

bool AComplexityManager::IsLoadNextSubSequence(int32 TilesPassed){
    // Check if there is a next SubSequence
    if(SubSequenceIndex >= (ActiveSequence.SubSequenceSettings.Num()-1)){
        return false;
    }
    // Check if increase NextSubSequenceIndex
    if((TilesPassed % NextSubSequenceIncrease) == 0){
        // Increase SubSeqeunce index
        SubSequenceIndex += 1;
        // Load next SubSequence
        LoadSubSequence();
        return true;
    }
    return false;
}

/*
* Takes a dynamic Material instance and adjusts it's parameters according to the
* current global material activation settings
*/
void AComplexityManager::AdjustDynamicMaterial(UMaterialInstanceDynamic* DynamicMaterial){
    // Set Parameters of DynamicMaterialInstance
    if(DynamicMaterial){    // Nullptr check
        // Color
        if(MaterialActivationPermutation.UseAlbedo){
            DynamicMaterial->SetScalarParameterValue("UseAlbedo", 1.f);
        } else {
            DynamicMaterial->SetScalarParameterValue("UseAlbedo", 0.f);
        }
        // Normals
        if(MaterialActivationPermutation.UseNormals){
            DynamicMaterial->SetScalarParameterValue("UseNormals", 1.f);
        } else {
            DynamicMaterial->SetScalarParameterValue("UseNormals", 0.f);
        }
        // Roughness
        if(MaterialActivationPermutation.UseRoughness){
            DynamicMaterial->SetScalarParameterValue("UseRoughness", 1.f);
        } else {
            DynamicMaterial->SetScalarParameterValue("UseRoughness", 0.f);
        }
        // Metallic
        if(MaterialActivationPermutation.UseMetallic){
            DynamicMaterial->SetScalarParameterValue("UseMetallic", 1.f);
        } else {
            DynamicMaterial->SetScalarParameterValue("UsetMetallic", 0.f);
        }
    }
}


/*
* Adjust a DynamicMaterial instance according to a given MaterialActivation
*/
void AComplexityManager::AdjustDynamicMaterial(UMaterialInstanceDynamic* DynamicMaterial, FMaterialActivationStruct MaterialActivation){
    // Set Parameters of DynamicMaterialInstance
    if(DynamicMaterial){    // Nullptr check
        // Color
        if(MaterialActivation.UseAlbedo){
            DynamicMaterial->SetScalarParameterValue("UseAlbedo", 1.f);
        } else {
            DynamicMaterial->SetScalarParameterValue("UseAlbedo", 0.f);
        }
        // Normals
        if(MaterialActivation.UseNormals){
            DynamicMaterial->SetScalarParameterValue("UseNormals", 1.f);
        } else {
            DynamicMaterial->SetScalarParameterValue("UseNormals", 0.f);
        }
        // Roughness
        if(MaterialActivation.UseRoughness){
            DynamicMaterial->SetScalarParameterValue("UseRoughness", 1.f);
        } else {
            DynamicMaterial->SetScalarParameterValue("UseRoughness", 0.f);
        }
        // Metallic
        if(MaterialActivation.UseMetallic){
            DynamicMaterial->SetScalarParameterValue("UseMetallic", 1.f);
        } else {
            DynamicMaterial->SetScalarParameterValue("UsetMetallic", 0.f);
        }
    }
}


/*
* Takes a dynamic material instance and adjusts its color
*/
void AComplexityManager::SetDynamicMaterialColor(UMaterialInstanceDynamic *DynamicMaterial, int32 index,
                        TMap<int32, FLinearColor>& MinColorMap, TMap<int32, FLinearColor>& MaxColorMap){
    if(DynamicMaterial){ // Nullptr check
        //const TPair<int32, FLinearColor>& pair : MinColorMap
        if(MinColorMap.Contains(index) && MaxColorMap.Contains(index)){ // might be an unnecessary check
            // Init new color
            FLinearColor color = FLinearColor::Black;
            // Convert color to HSV
            color = color.LinearRGBToHSV();
            // Get Random Hue from 'min' and 'max' color
            color.R = GetRandomnessManager()->GetFloatInRange(
                        MinColorMap[index].LinearRGBToHSV().R,
                        MaxColorMap[index].LinearRGBToHSV().R);
            // Get Random Value
            color.G = GetRandomnessManager()->GetFloatInRange(
                        MinColorMap[index].LinearRGBToHSV().G,
                        MaxColorMap[index].LinearRGBToHSV().G);
            // Get Random Saturation
            color.B = GetRandomnessManager()->GetFloatInRange(
                        MinColorMap[index].LinearRGBToHSV().B,
                        MaxColorMap[index].LinearRGBToHSV().B);

            // Convert color back to LinearRGB
            color = color.HSVToLinearRGB();

            DynamicMaterial->SetVectorParameterValue("BlendColor", color);
        }
    }

}

int32 AComplexityManager::GetNextEnvironmentStructIndex(){
    return 0;
}

/*
* This function should be called during "Setup" phase of the simulation to get
* the callback to the environment manager
*/
void AComplexityManager::SetEnvironmentManagerCallback(AEnvironmentManager *CallbackManager){
    EnvironmentManagerCallback = CallbackManager;
}

ARandomnessManager* AComplexityManager::GetRandomnessManager(){
    if(GetWorld()){
        if(GetWorld()->GetAuthGameMode()){
            AEndlessRunnerGameMode* GameModeCallback = (AEndlessRunnerGameMode*)GetWorld()->GetAuthGameMode();
            ARandomnessManager* randomnessManager = GameModeCallback->RandomnessManager;
            return randomnessManager;
        }
    }
    return nullptr;
}

//FIXME: stupid function does not scale at all
// should be an array of structs combined with the enum
ETerrainTypes AComplexityManager::GetTerrainType(){
    // Add up all probabilities
    float probabilityMass = ParkProbabiltiy + UrbanProbability + SubUrbanProbability;
    // Draw a random number from 0 to probabilityMass
    float rand = 0.f;
    if(GetRandomnessManager()){
        rand = GetRandomnessManager()->GetFloatInRange(0.f, probabilityMass);
    }
    // Check rand against all probabilities
    float mass = 0.f;
    // Check PARK
    mass += ParkProbabiltiy;
    if(rand <= mass){
        return ETerrainTypes::PARK;
    }
    // Check URBAN
    mass += UrbanProbability;
    if(rand <= mass){
        return ETerrainTypes::URBAN;
    }
    mass += SubUrbanProbability;
    if(rand <= mass){
        return ETerrainTypes::SUBURBAN;
    }
    UE_LOG(LogTemp, Error, TEXT("No Terrain was selected -> fallback to PARK terrain"));
    return ETerrainTypes::PARK;
}

/*
* A sucky way to get bools by object name...
*/
bool AComplexityManager::IsObjectSpawning(FString ObjectName){
    // Check if there is a struct for current permutation
    if(ObjectName == "Building"){
        return ObjectPermutation.Building;
    }
    else if(ObjectName == "Car"){
        return ObjectPermutation.Car;
    }
    else if(ObjectName == "Streetlamp" || ObjectName == "Lamp"){
        return ObjectPermutation.Streetlamp;
    }
    else if(ObjectName == "People"){
        return ObjectPermutation.People;
    }
    else if(ObjectName == "Tree"){
        return ObjectPermutation.Tree;
    } else{
        UE_LOG(LogTemp, Warning, TEXT("Actor with name: %s, is not know"), *ObjectName);
        return false;
    }
}

void AComplexityManager::SetEnvironmentSettings(ABaseTile *tile){
    tile->EnvironmentSettingStruct.EnvironmentStruct = EnvironmentSettings;
    UE_LOG(LogTemp, Log, TEXT("Wrote environemt"));
    tile->EnvironmentSettingStruct.ObjectApperanceStruct = ObjectPermutation;
    UE_LOG(LogTemp, Log, TEXT("Wrote object appearnace"));
    tile->EnvironmentSettingStruct.ShaderStruct = MaterialActivationPermutation;
    UE_LOG(LogTemp, Log, TEXT("Wrote shader"));
    UE_LOG(LogTemp, Log, TEXT("Wrote new environemt to tile"));
}

void AComplexityManager::RegisterToGameMode(){
    if(GetWorld()){
        if(GetWorld()->GetAuthGameMode()){
            AEndlessRunnerGameMode* gameModeCallback = (AEndlessRunnerGameMode*)GetWorld()->GetAuthGameMode();
            gameModeCallback->ComplexityManager = this;
            UE_LOG(LogTemp, Warning, TEXT("ComplexityManager: Registered to GameMode"));
        }
    }
}