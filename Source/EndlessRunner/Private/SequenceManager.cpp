// Fill out your copyright notice in the Description page of Project Settings.


#include "SequenceManager.h"

#include "Json.h"
#include "JsonObjectConverter.h"
#include "Misc/Paths.h"

#include "Common/CommonStructs.h"
#include "EndlessRunner_GameInstance.h"

#include "RandomnessManager.h"
#include "SegmentationManager.h"
#include "EnvironmentManager.h"
#include "ComplexityManager.h"
#include "TileManager.h"

#include "EndlessRunnerGameMode.h"

// Sets default values
ASequenceManager::ASequenceManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

ASequenceManager::~ASequenceManager(){
    // Close JSONSequence String
    JSONSequenceString += "]";
    SaveSequenceJSON();
}

// Called when the game starts or when spawned
void ASequenceManager::BeginPlay()
{
	Super::BeginPlay();

    // Register to GameMode
    RegisterToGameMode();
    UE_LOG(LogTemp, Error, TEXT("SequenceManager: BeginPlay"));

    // Read JsonFilePaths from GameInstance
    JsonSequenceFilePaths = Cast<UEndlessRunner_GameInstance>(GetGameInstance())->JsonSequencePaths;
    UE_LOG(LogTemp, Warning, TEXT("Received %d JsonPath(s)"), JsonSequenceFilePaths.Num());
    if(JsonSequenceFilePaths.Num()>0){
        ReadFromJson = true;
        UE_LOG(LogTemp, Warning, TEXT("Reading sequence configuration from json!"));
    } else {
        UE_LOG(LogTemp, Error, TEXT("Defaulting to standart showcase sequence because no configuration was found!"));
    }

    // Request Initial Sequence Reset/Setup
    IsResetSequence = true;
	
}

// Called every frame
void ASequenceManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    if(IsResetSequence){
        UE_LOG(LogWindows, Warning, TEXT("SequenceManager: First Tick"))
        // Load data of active sequence to this manager
        LoadSequenceData();
        // Load Sequence to Managers
        PropagateSequenceSetup();
        // Init Tiles
        GetTileManager()->InitialTileSpawn();
        IsResetSequence = false;
    }

    // TODO: Think thoroughly about whethere this is the appropriate way to communicate with managers!?!?!
    if(GetSegmentationManager()->IsSequenceComplete){
        // Update SequenceCounter etc.
        SetNextSequenceModule();
        // Load the data of the next sequence to this manager
        LoadSequenceData();
        // Load next Sequence data to Managers
        PropagateSequenceSetup();
    }

}

/*
* Loads the data of the ActiveSequenceSetup as provided by DataAsset or JsonFile
*/
void ASequenceManager::LoadSequenceData(){
    // Loading from Json
    if(ReadFromJson){
        ActiveSequenceSetup = ReadSequencesFromJsonFile(JsonFile);
        /* DEBUG
        UE_LOG(LogTemp, Warning, TEXT("Read sequence from json"));
        FString jsonConversionTest;
        FJsonObjectConverter::UStructToJsonObjectString(ActiveSequenceSetup, jsonConversionTest, 0, 0);
        UE_LOG(LogTemp, Warning, TEXT("JsonFromStruct:"));
        UE_LOG(LogTemp, Warning, TEXT("%s"), *jsonConversionTest);
        */
        return;
    } else{
        // Loading from DataAsset
        ActiveSequenceSetup = SequenceModules[ActiveSequenceIndex]->Sequence;
        // DEBUG: Convert UStruct to JSON String
        FString jsonConversionTest;
        FJsonObjectConverter::UStructToJsonObjectString(ActiveSequenceSetup, jsonConversionTest, 0, 0);
        UE_LOG(LogTemp, Warning, TEXT("JsonFromStruct:"));
        UE_LOG(LogTemp, Warning, TEXT("%s"), *jsonConversionTest);
        
    }
    // Sanity Check: Is there any SubSequence defined?
    if(ActiveSequenceSetup.SubSequenceSettings.Num() == 0){
        UE_LOG(LogTemp, Fatal, TEXT("No SubSequences defined!"));
    }
    return;
}

/*
 * Load Sequence varaibles from this ActiveSeqeunceSettings to other Managers
 * This needs to be done BEFORE any tile is spawned!
 */
void ASequenceManager::PropagateSequenceSetup(){
    // Write Data from current SequenceModule(DataAsset) to respective Manager
    // ComplexityManager
    GetComplexityManager()->LoadSequenceModule(ActiveSequenceSetup);
    //GetComplexityManager()->SetInternalVariables(); //TODO: remove
    // EnvironmentManager
    // is subject to ComplexityManager
    // TileManager
    GetTileManager()->LoadSequenceModule(ActiveSequenceSetup);
    // SegmentationManager
    GetSegmentationManager()->LoadSequenceModule(ActiveSequenceSetup.CaptureSettings, SequenceCounter);
    // RandomnessManger
    GetRandomnessManager()->ResetRandomEngine(ActiveSequenceSetup.RandomSeed);
    
    // Write Scene Setup to JSON
    InitSequenceJSON();
    WriteSegmentationJSON();
}


void ASequenceManager::SetNextSequenceModule(){
    // TODO: Prepare for multiple sequences
    if(GetWorld()){
        if(GetWorld()->GetAuthGameMode()){
            AEndlessRunnerGameMode* GameModeCallback = (AEndlessRunnerGameMode*)GetWorld()->GetAuthGameMode();
            GameModeCallback->QuitGame();
            // WARNING: GameQuit Command is not immediate! Next Tick will be processed
            return;
        }
    }

    /*
    UE_LOG(LogTemp, Log, TEXT("SetNextSequenceModule"));
    // Increase sequence couter
    SequenceCounter += 1;
    SequenceCounterSpecific += 1;
    // Check if Module is running again
    // Increase active sequence index if needed
    int32 test = SequenceModules[ActiveSequenceIndex]->SequenceRepetitions;
    UE_LOG(LogTemp, Log, TEXT("Repetitions: %d"), test);
    if(SequenceCounterSpecific >= SequenceModules[ActiveSequenceIndex]->SequenceRepetitions){
        UE_LOG(LogTemp, Log, TEXT("Loading new Module"))
        // Check if there is another SequenceModule
        if((ActiveSequenceIndex+1) >= SequenceModules.Num()-1){
            // No more Sequences -> Simulation Done
            UE_LOG(LogTemp, Log, TEXT("Calling for Simulation Close.."));
            if(GetWorld()){
                if(GetWorld()->GetAuthGameMode()){
                    AEndlessRunnerGameMode* GameModeCallback = (AEndlessRunnerGameMode*)GetWorld()->GetAuthGameMode();
                    GameModeCallback->QuitGame();
                    // WARNING: GameQuit Command is not immediate! Next Tick will be processed
                    return;
                }
            }
        }
        // Switch Active Sequence Index
        ActiveSequenceIndex += 1;
        // Reset SequenceCounterSpecific
        SequenceCounterSpecific = 0;
    }
    */
}

/*
 * Writes the meta data (configurations) of and during the sequence to a
 * JSON file
 */
void ASequenceManager::InitSequenceJSON(){
    // Init JSON String
    JSONSequenceString = "[";

    FString tempJSONString = "";
    TSharedRef <TJsonWriter<TCHAR>> JsonWriter = TJsonWriterFactory<>::Create(&tempJSONString);
    // Write data
    JsonWriter->WriteObjectStart();
        // Image Counter
        JsonWriter->WriteObjectStart("Sequence");
            JsonWriter->WriteValue("ImageCounter", GetSegmentationManager()->NumFramesCaptured);
        JsonWriter->WriteObjectEnd();
        // Object Complexity
        JsonWriter->WriteObjectStart("ObjectPermutation");
        FObjectApperanceStruct currObjStruct = GetComplexityManager()->ObjectPermutation;
            JsonWriter->WriteValue("Building", currObjStruct.Building);
            JsonWriter->WriteValue("Car", currObjStruct.Car);
            JsonWriter->WriteValue("Streetlamp", currObjStruct.Streetlamp);
            JsonWriter->WriteValue("People", currObjStruct.People);
            JsonWriter->WriteValue("Tree", currObjStruct.Tree);
        JsonWriter->WriteObjectEnd();
        // Material Complexity
        JsonWriter->WriteObjectStart("MaterialActivationPermutation");
        FMaterialActivationStruct currShaderStruct = GetComplexityManager()->MaterialActivationPermutation;
            JsonWriter->WriteValue("UseAlbedo", currShaderStruct.UseAlbedo);
            JsonWriter->WriteValue("UseNormals", currShaderStruct.UseNormals);
            JsonWriter->WriteValue("UseRoughness", currShaderStruct.UseRoughness);
            JsonWriter->WriteValue("UseMetallic", currShaderStruct.UseMetallic);
        JsonWriter->WriteObjectEnd();
        // Environment Complexity
        JsonWriter->WriteObjectStart("EnvironmentSettings");
        FEnvironmentStruct currEnvironmentStruct = GetComplexityManager()->EnvironmentSettings;
            JsonWriter->WriteValue("SunIntensity", currEnvironmentStruct.SunIntenstiy);
            JsonWriter->WriteValue("SkyLightIntensity", currEnvironmentStruct.SkyLightIntensity);
            JsonWriter->WriteValue("IsSunMoving", currEnvironmentStruct.IsSunMoving);
            JsonWriter->WriteValue("ToD: Hours", currEnvironmentStruct.DayTime.Hours);
            JsonWriter->WriteValue("ToD: Minutes", currEnvironmentStruct.DayTime.Minutes);
            JsonWriter->WriteValue("SunSpeedMultiplier", currEnvironmentStruct.SunSpeedMultiplier);
            JsonWriter->WriteValue("SunBrightness", currEnvironmentStruct.SunBrightness);
            JsonWriter->WriteValue("CloudOpacity", currEnvironmentStruct.CloudOpacity);
            JsonWriter->WriteValue("CouldSpeed", currEnvironmentStruct.CloudSpeed);
            JsonWriter->WriteValue("FogDensity", currEnvironmentStruct.FogDenstiy);
            JsonWriter->WriteValue("FogStartDistance", currEnvironmentStruct.FogStartDistance);
            JsonWriter->WriteValue("FogHeightFalloff", currEnvironmentStruct.FogHeightFalloff);
            JsonWriter->WriteValue("IsRaining", currEnvironmentStruct.IsRaining);
            JsonWriter->WriteValue("IsSnowing", currEnvironmentStruct.IsSnowing);
            JsonWriter->WriteValue("ParticleBrightness", currEnvironmentStruct.ParticleBrightness);
            JsonWriter->WriteValue("ParticleDensity", currEnvironmentStruct.ParticleDensity);
            JsonWriter->WriteValue("RainImpactDensity", currEnvironmentStruct.RainImpactDenstiy);
            JsonWriter->WriteValue("MaterialAdjustmentStrength", currEnvironmentStruct.MaterialAdjustmentStrength);
            
        JsonWriter->WriteObjectEnd();
    JsonWriter->WriteObjectEnd();
    JsonWriter->Close();

    JSONSequenceString += tempJSONString;

    SaveSequenceJSON();
}

void ASequenceManager::AddToSequenceJSON(FEnvironmentSettingStruct SettingsStruct){
    FString tempJSONString = "";
    TSharedRef <TJsonWriter<TCHAR>> JsonWriter = TJsonWriterFactory<>::Create(&tempJSONString);
    // Write data
    JsonWriter->WriteObjectStart();
        // Image Counter
        JsonWriter->WriteObjectStart("Sequence");
            JsonWriter->WriteValue("ImageCounter", GetSegmentationManager()->NumFramesCaptured);
        JsonWriter->WriteObjectEnd();
        // Object Complexity
        JsonWriter->WriteObjectStart("ObjectPermutation");
        FObjectApperanceStruct currObjStruct = SettingsStruct.ObjectApperanceStruct;
            JsonWriter->WriteValue("Building", currObjStruct.Building);
            JsonWriter->WriteValue("Car", currObjStruct.Car);
            JsonWriter->WriteValue("Streetlamp", currObjStruct.Streetlamp);
            JsonWriter->WriteValue("People", currObjStruct.People);
            JsonWriter->WriteValue("Tree", currObjStruct.Tree);
        JsonWriter->WriteObjectEnd();
        // Material Complexity
        JsonWriter->WriteObjectStart("MaterialActivationPermutation");
        FMaterialActivationStruct currShaderStruct = SettingsStruct.ShaderStruct;
            JsonWriter->WriteValue("UseAlbedo", currShaderStruct.UseAlbedo);
            JsonWriter->WriteValue("UseNormals", currShaderStruct.UseNormals);
            JsonWriter->WriteValue("UseRoughness", currShaderStruct.UseRoughness);
            JsonWriter->WriteValue("UseMetallic", currShaderStruct.UseMetallic);
        JsonWriter->WriteObjectEnd();
        // Environment Complexity
        JsonWriter->WriteObjectStart("EnvironmentSettings");
        FEnvironmentStruct currEnvironmentStruct = SettingsStruct.EnvironmentStruct;
            JsonWriter->WriteValue("SunIntensity", currEnvironmentStruct.SunIntenstiy);
            JsonWriter->WriteValue("SkyLightIntensity", currEnvironmentStruct.SkyLightIntensity);
            JsonWriter->WriteValue("IsSunMoving", currEnvironmentStruct.IsSunMoving);
            JsonWriter->WriteValue("ToD: Hours", currEnvironmentStruct.DayTime.Hours);
            JsonWriter->WriteValue("ToD: Minutes", currEnvironmentStruct.DayTime.Minutes);
            JsonWriter->WriteValue("SunSpeedMultiplier", currEnvironmentStruct.SunSpeedMultiplier);
            JsonWriter->WriteValue("SunBrightness", currEnvironmentStruct.SunBrightness);
            JsonWriter->WriteValue("CloudOpacity", currEnvironmentStruct.CloudOpacity);
            JsonWriter->WriteValue("CouldSpeed", currEnvironmentStruct.CloudSpeed);
            JsonWriter->WriteValue("FogDensity", currEnvironmentStruct.FogDenstiy);
            JsonWriter->WriteValue("FogStartDistance", currEnvironmentStruct.FogStartDistance);
            JsonWriter->WriteValue("FogHeightFalloff", currEnvironmentStruct.FogHeightFalloff);
            JsonWriter->WriteValue("FogMaxOpacity", currEnvironmentStruct.FogMaxOpacity);
            JsonWriter->WriteValue("IsRaining", currEnvironmentStruct.IsRaining);
            JsonWriter->WriteValue("IsSnowing", currEnvironmentStruct.IsSnowing);
            JsonWriter->WriteValue("ParticleBrightness", currEnvironmentStruct.ParticleBrightness);
            JsonWriter->WriteValue("ParticleDensity", currEnvironmentStruct.ParticleDensity);
            JsonWriter->WriteValue("RainImpactDensity", currEnvironmentStruct.RainImpactDenstiy);
            JsonWriter->WriteValue("MaterialAdjustmentStrength", currEnvironmentStruct.MaterialAdjustmentStrength);
        JsonWriter->WriteObjectEnd();
    JsonWriter->WriteObjectEnd();

    JsonWriter->Close();

    // Add to JSONSequence
    JSONSequenceString += ",\n" + tempJSONString; // ,\n: needed for proper JSON formatting

    SaveSequenceJSON();
}

void ASequenceManager::SaveSequenceJSON(){
    //UE_LOG(LogTemp, Log, TEXT("JSON: %s"), *JSONSequenceString);
    FString filename = FPaths::ProjectSavedDir() + "Sequence.json";
    FFileHelper::SaveStringToFile(JSONSequenceString, *filename);
    
}

/*
* Code by: https://www.orfeasel.com/parsing-json-files/
*/
FSequence ASequenceManager::ReadSequencesFromJsonFile(FString JsonFilePath){
    
    FString jsonFileString;
    FString filename = "";
    if(JsonSequenceFilePaths.Num() > 0){ // if any json files loaded
        filename = JsonSequenceFilePaths[ActiveSequenceIndex];
    } else{
        filename = FPaths::ProjectDir() + JsonFilePath;
        UE_LOG(LogTemp, Warning, TEXT("Reading json file: %s"), *filename);
    }
    
    FFileHelper::LoadFileToString(jsonFileString, *filename);
    if(jsonFileString.IsEmpty()){ // check json file reads
        UE_LOG(LogTemp, Error, TEXT("JsonFile was empty!"));
        FSequence emptySequence;
        return emptySequence;
    }
    UE_LOG(LogTemp, Warning, TEXT("Got input json string: \n %s"), *jsonFileString);
    FSequence sequence;
    FJsonObjectConverter::JsonObjectStringToUStruct(jsonFileString, &sequence, 0, 0);

    // DEBUG: Convert UStruct to JSON String
    /*
    FString jsonConversionTest;
    FJsonObjectConverter::UStructToJsonObjectString(ActiveSequenceSetup, jsonConversionTest, 0, 0);
    UE_LOG(LogTemp, Warning, TEXT("JsonFromStruct:"));
    UE_LOG(LogTemp, Warning, TEXT("%s"), *jsonConversionTest);
    */
    return sequence;
}

void ASequenceManager::WriteSegmentationJSON(){
    FString JSONSegmentationString = "";

    FString tempJsonString = "";
    TSharedRef <TJsonWriter<TCHAR>> JsonWriter = TJsonWriterFactory<>::Create(&tempJsonString);

    // Write data
    JsonWriter->WriteObjectStart();
        JsonWriter->WriteObjectStart("ObjectClassMapping");
        if(GetSegmentationManager()->IsInstanceSegmentation){ // write min values
            for(auto& Elem : GetSegmentationManager()->InstanceSegmentationDict){
                JsonWriter->WriteValue(*Elem.Key, Elem.Value.Min);
            }
        } else {
            for(auto& Elem : GetSegmentationManager()->SegmentationDict){
                JsonWriter->WriteValue(*Elem.Key, Elem.Value);
            }
        }
        JsonWriter->WriteObjectEnd();
    JsonWriter->WriteObjectEnd();
    JsonWriter->Close();

    if(GetSegmentationManager()->IsInstanceSegmentation){ // also write max values
        JsonWriter->WriteObjectStart();
        JsonWriter->WriteObjectStart("ObjectClassMapping");
        
        for(auto& Elem : GetSegmentationManager()->InstanceSegmentationDict){
            JsonWriter->WriteValue(*Elem.Key, Elem.Value.Max);
        }
         
        JsonWriter->WriteObjectEnd();
        JsonWriter->WriteObjectEnd();
        JsonWriter->Close();
    }

    if(GetSegmentationManager()->IsInstanceSegmentation){
        JSONSegmentationString = "[" + tempJsonString + "]";
    } else {
        JSONSegmentationString = tempJsonString;
    }

    // Save to file
    FString filename = FPaths::ProjectSavedDir() + "Segmentation.json";
    FFileHelper::SaveStringToFile(JSONSegmentationString, *filename);
}


// Get Complexity Manger
AComplexityManager* ASequenceManager::GetComplexityManager(){
    if(GetWorld()){
        if(GetWorld()->GetAuthGameMode()){
            AEndlessRunnerGameMode* GameModeCallback = (AEndlessRunnerGameMode*)GetWorld()->GetAuthGameMode();
            AComplexityManager* complexityManager = GameModeCallback->ComplexityManager;
            return complexityManager;
        }
    }
    return nullptr;
}
// Get Randomness Manager
ARandomnessManager* ASequenceManager::GetRandomnessManager(){
    if(GetWorld()){
        if(GetWorld()->GetAuthGameMode()){
            AEndlessRunnerGameMode* GameModeCallback = (AEndlessRunnerGameMode*)GetWorld()->GetAuthGameMode();
            ARandomnessManager* randomnessManager = GameModeCallback->RandomnessManager;
            return randomnessManager;
        }
    }
    return nullptr;
}
// Get Tile Manager
ATileManager* ASequenceManager::GetTileManager(){
    if(GetWorld()){
        if(GetWorld()->GetAuthGameMode()){
            AEndlessRunnerGameMode* GameModeCallback = (AEndlessRunnerGameMode*)GetWorld()->GetAuthGameMode();
            ATileManager* tileManager = GameModeCallback->TileManager;
            return tileManager;
        }
    }
    return nullptr;
}
// Get Environment Manager
AEnvironmentManager* ASequenceManager::GetEnvironmentManager(){
    if(GetWorld()){
        if(GetWorld()->GetAuthGameMode()){
            AEndlessRunnerGameMode* GameModeCallback = (AEndlessRunnerGameMode*)GetWorld()->GetAuthGameMode();
            AEnvironmentManager* environmentManager = GameModeCallback->EnvironmentManager;
            return environmentManager;
        }
    }
    return nullptr;
}
// Get Segmentation Manager
ASegmentationManager* ASequenceManager::GetSegmentationManager(){
    if(GetWorld()){
        if(GetWorld()->GetAuthGameMode()){
            AEndlessRunnerGameMode* GameModeCallback = (AEndlessRunnerGameMode*)GetWorld()->GetAuthGameMode();
            ASegmentationManager* segmentationManager = GameModeCallback->SegmentationManager;
            return segmentationManager;
        }
    }
    return nullptr;
}

void ASequenceManager::RegisterToGameMode(){
    if(GetWorld()){
        if(GetWorld()->GetAuthGameMode()){
            AEndlessRunnerGameMode* gameModeCallback = (AEndlessRunnerGameMode*)GetWorld()->GetAuthGameMode();
            gameModeCallback->SequenceManager = this;
        }
    }
}   

