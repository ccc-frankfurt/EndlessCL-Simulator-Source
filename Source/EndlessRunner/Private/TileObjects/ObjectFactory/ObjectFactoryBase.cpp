// Fill out your copyright notice in the Description page of Project Settings.


#include "ObjectFactoryBase.h"

#include "HAL/FileManagerGeneric.h"
#include "HAL/FileManager.h"

#include "Misc/Paths.h"

#include "Engine/World.h"

#include "EndlessRunnerGameMode.h"
#include "Runtime/Engine/Public/EngineUtils.h"

#include "RandomnessManager.h"
#include "ComplexityManager.h"

// Sets default values
AObjectFactoryBase::AObjectFactoryBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

/*
*
*/
UClass* AObjectFactoryBase::GetAssetClass(FString BasePath, FString ClassName){
    // Get current object complexity
    int32 currComplexity = GetComplexityManager()->ObjectComplexity;
    // Get path to asset
    FString assetPath = GetRandomAssetFromPathWithComplexity(BasePath, ClassName, currComplexity);
    // Check if assetPath is empty and retun nullptr if nothing got found
    if(assetPath.IsEmpty()){
        return nullptr;
    }
    // Build class reference string
    FString classRefString = BuildClassReferenceString(assetPath);
    return LoadObject<UClass>(this, *classRefString);
}


/*
*
*/
UClass* AObjectFactoryBase::GetAssetClass(FString ClassName){
    // Get current object complexity
    int32 currComplexity = GetComplexityManager()->ObjectComplexity;
    // Get path to asset from TileManager
    FString assetBasePath = GetTileManager()->TileObjectSampleDirectoryMap[ClassName];
    FString assetPath = GetRandomAssetFromPathWithComplexity(assetBasePath, ClassName, currComplexity);
    if(assetPath.IsEmpty()){
        return nullptr;
    }
    FString classRefString = BuildClassReferenceString(assetPath);
    return LoadObject<UClass>(this, *classRefString);
}


/*
* Returns the UClass reference of an asset sampled by a categorical distribution
*/
UClass* AObjectFactoryBase::GetAssetClassCategorical(FString ClassName){
    // Get the base path for the asset (type)
    FString assetBasePath = GetAssetBasePathWithComplexity(ClassName, GetComplexityManager()->ObjectComplexity);
    UE_LOG(LogTemp, Warning, TEXT("assetBasePath: %s"), *assetBasePath);
    

    FString assetPath = GetAssetNameCategorical(assetBasePath);
    UE_LOG(LogTemp, Warning, TEXT("assetPath: %s"), *assetPath);
    FString classRefString = BuildClassReferenceString(assetPath);
    //UE_LOG(LogTemp, Warning, TEXT("classRefString: %s"), *classRefString);
    return LoadObject<UClass>(this, *classRefString);
}


FString AObjectFactoryBase::GetAssetNameCategorical(FString BasePath){
    // Get FileManager
    IFileManager &fileManager = IFileManager::Get();

    // Get all directories and files
    TArray<FString> fileNameArray; 
    FString searchPath = FPaths::Combine(*FPaths::ProjectContentDir(), BasePath) + "*"; // Wildcard "*" is needed, otherwise directories are not found
    fileManager.FindFiles(fileNameArray, *searchPath, true, true);
    // Check if empty
    //UE_LOG(LogTemp, Warning, TEXT("FilesFound: %d"), fileNameArray.Num());
    if(fileNameArray.Num() == 0){ 
        return "";
    }
    TArray<FString> finalFileNameArray; 
    for(FString fileName : fileNameArray){
        //UE_LOG(LogTemp, Warning, TEXT("fileName: %s"), *fileName);
        if(fileName.EndsWith(".uexp")){
            continue;
        }
        finalFileNameArray.Add(fileName);
    }

    // Init categorical weights
    TArray<float> categoricalWeights;
    categoricalWeights.Init(1.f, finalFileNameArray.Num());

    // Get the keys for the predefined categorical weights
    TArray<FString> categoricalWeightKeys;
    GetRandomnessManager()->CategoricalSamplingWeights.GetKeys(categoricalWeightKeys);

    // Set categorical weights from predefined weights in RandomnessManager
    for(size_t i=0;i<finalFileNameArray.Num();i++){
        for(FString key : categoricalWeightKeys){
            if(key.Equals(finalFileNameArray[i])){
                categoricalWeights[i] = GetRandomnessManager()->CategoricalSamplingWeights[key];
            }
        }
    }

    // Sample using categorical distribution
    int32 selectedIndex = GetRandomnessManager()->SampleCategoricalDistribution(categoricalWeights);
    // Key corresponding entry
    FString selectedElement = finalFileNameArray[selectedIndex];
    //UE_LOG(LogTemp, Warning, TEXT("Selected Element: %s"), *selectedElement);
    // Construct new BasePath
    FString newBasePath = BasePath;
    // Check if UAsset
    if(selectedElement.EndsWith(".uasset")){
        newBasePath += selectedElement;
        newBasePath.RemoveFromEnd(".uasset");
        return newBasePath;
    }
    
    // Extend path by selected element
    newBasePath+= selectedElement + "/";
    //UE_LOG(LogTemp, Warning, TEXT("new BasePath: %s"), *newBasePath);
    // Recursive function call 
    newBasePath = GetAssetNameCategorical(newBasePath);
    return newBasePath;
}


/*
* Returns the BasePath to the given ClassName according to provided Complexity number.
*/
FString AObjectFactoryBase::GetAssetBasePathWithComplexity(FString ClassName, int32 Complexity){
    // Create a filemanager instance
    //FFileManagerGeneric fileManager;
    //fileManager.SetSandboxEnabled(true);
    IFileManager &fileManager = IFileManager::Get();

    // Get asset's base path
    FString assetBasePath = GetTileManager()->TileObjectSampleDirectoryMap[ClassName];

    // Find asset directory with given AssetComplexity or alternatively highest possible complexity
    int32 currComplexity = Complexity;
    FString discPath = assetBasePath + FString::FromInt(currComplexity) + "/" + ClassName + "/";
    FString searchPath = FPaths::Combine(*FPaths::ProjectContentDir(), discPath);

    // Check if path leads to a directory, if not: reduce complexity 
    while(!fileManager.DirectoryExists(*searchPath) && (currComplexity >=0)){
        currComplexity -= 1;
        discPath = assetBasePath + FString::FromInt(currComplexity) + "/" + ClassName + "/";
        searchPath = FPaths::Combine(*FPaths::ProjectContentDir(), discPath);
    }
    // If nothing found return empty string
    if(currComplexity == -1){
        UE_LOG(LogTemp, Warning, TEXT("ObjectFactoryBase: No objects found in path: %s"), *searchPath);
        return "";
    }

    return discPath;
}


FString AObjectFactoryBase::BuildClassReferenceString(FString AssetPath){
    FString classReferenceString = "";
    TArray<FString> splitStringArray;
    AssetPath.ParseIntoArray(splitStringArray, TEXT("/"), false);
    FString assetName = splitStringArray.Last();
    classReferenceString = BlueprintPrefix + AssetPath + "." + assetName + BlueprintSearchEnd;
    return classReferenceString;
}

FString AObjectFactoryBase::GetRandomAssetFromPathWithComplexity(FString BasePath, FString ClassName, int32 Complexity){
    TArray<FString> fileNameArray;
    FFileManagerGeneric fileManager;
    fileManager.SetSandboxEnabled(true);

    int32 currComplexity = Complexity;
    FString discPath = "";
    FString searchPath = "";

    // Fall back to the highest (possible) complexity if desired complexity value is too high 
    while((fileNameArray.Num() == 0) && (currComplexity >= 0)){
        discPath = BasePath + FString::FromInt(currComplexity) + "/" + ClassName + "/";
        searchPath = FPaths::Combine(*FPaths::ProjectContentDir(), discPath);
        fileManager.FindFiles(fileNameArray, *searchPath);
        currComplexity -= 1;
    }
    if((currComplexity == -1) && (fileNameArray.Num() == 0)){
        UE_LOG(LogTemp, Warning, TEXT("ObjectFactoryBase: No objects found in path: %s"), *searchPath);
        return "";
    }
    int32 randomIndex = GetRandomnessManager()->GetIntInRange(0, fileNameArray.Num()-1);
    FString assetName = fileNameArray[randomIndex];
    assetName.RemoveFromEnd(".uasset");

    return discPath + assetName;
}

ATileManager* AObjectFactoryBase::GetTileManager(){
    if(GetWorld()){
        if(GetWorld()->GetAuthGameMode()){
             AEndlessRunnerGameMode* GameModeCallback = (AEndlessRunnerGameMode*)GetWorld()->GetAuthGameMode();
             ATileManager* tileManager = GameModeCallback->TileManager;
             return tileManager;
        }
    }
    UE_LOG(LogTemp, Error, TEXT("ObjectFactoryBase: GetTileManager: No World Found!"));
    return nullptr;
}

ARandomnessManager* AObjectFactoryBase::GetRandomnessManager(){
    if(GetWorld()){
        if(GetWorld()->GetAuthGameMode()){
            AEndlessRunnerGameMode* GameModeCallback = (AEndlessRunnerGameMode*)GetWorld()->GetAuthGameMode();
            ARandomnessManager* randomnessManager = GameModeCallback->RandomnessManager;
            return randomnessManager;
        }
    }
    UE_LOG(LogTemp, Error, TEXT("ObjectFactoryBase: GetRandomnessManager: No World Found!"));
    return nullptr;
}


AComplexityManager* AObjectFactoryBase::GetComplexityManager(){
    if(GetWorld()){
        if(GetWorld()->GetAuthGameMode()){
            AEndlessRunnerGameMode* GameModeCallback = (AEndlessRunnerGameMode*)GetWorld()->GetAuthGameMode();
            AComplexityManager* complexityManager = GameModeCallback->ComplexityManager;
            return complexityManager;
        }
    }
    UE_LOG(LogTemp, Error, TEXT("ObjectFactoryBase: GetComplexityManager: No World Found!"));
    return nullptr;
}

