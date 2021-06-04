// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

class ARandomnessManager;
class AComplexityManager;
class ATileManager;

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ObjectFactoryBase.generated.h"

UCLASS()
class ENDLESSRUNNER_API AObjectFactoryBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AObjectFactoryBase();

protected:
    FString BlueprintPrefix = "Class'/Game";
    FString BlueprintSearchEnd = "_C'";

    FString FactoryObjectName = "";

    FString FactorySamplePath = "";

protected:
    ARandomnessManager* GetRandomnessManager();
    AComplexityManager* GetComplexityManager();
    ATileManager* GetTileManager();

    FString GetAssetBasePathWithComplexity(FString ClassName, int32 Complexity);

    FString BuildClassReferenceString(FString AssetPath);

    FString GetAssetNameCategorical(FString BasePath);

public:	
    UClass* GetAssetClass(FString ClassName);
    UClass* GetAssetClass(FString BasePath, FString ClassName);

    FString GetRandomAssetFromPathWithComplexity(FString BasePath, FString ClassName, int32 Complexity);
    UClass* GetAssetClassCategorical(FString ClassName);
	
};
