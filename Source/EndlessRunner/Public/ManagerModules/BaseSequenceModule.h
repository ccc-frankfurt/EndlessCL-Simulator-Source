// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Common/CommonStructs.h"

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BaseSequenceModule.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class ENDLESSRUNNER_API UBaseSequenceModule : public UDataAsset
{
	GENERATED_BODY()
	
public:
    // How many runs shall be done with the settings specified in this module
    //UPROPERTY(EditAnywhere, Category="SequenceSettings")
    int32 SequenceRepetitions = 1;

    //Random seeds for the individual sequences
    //if there are loss seeds specified than SequenceRepetitions
    //seeds will be drawn at random.
    UPROPERTY(EditAnywhere, Category="SequenceSettings")
    TArray<int32> SequenceSeeds;
    
    UPROPERTY(EditAnywhere, Category="SequenceSettings")
    int32 MaxNumberOfTiles  = 100;

    UPROPERTY(EditAnywhere, Category="ObjectComplexity")
    int32 StartObjectComplexity = 0;
    UPROPERTY(EditAnywhere, Category="ObjectComplexity")
    int32 ObjectComplexityMax = 3;
    // Next ObjectComplextiy increase in Seconds
    UPROPERTY(EditAnywhere, Category="ObjectComplexity", meta=(ClampMin="1.0", UIMin="1.0"))
    int32 NextObjectComplexityIncrease = 3;

    UPROPERTY(EditAnywhere, Category="MaterialComplexity")
    int32 MaterialComplexityMax = 3;

    UPROPERTY(EditAnywhere, Category="ObjectPermutation")
    int32 StartObjectPermutationNumber = 0;
    // Next ObjectPermutationIncrease in Seconds
    UPROPERTY(EditAnywhere, Category="ObjectPermutation", meta=(ClampMin="1.0", UIMin="1.0"))
    int32 NextObjectPermutationIncrease = 1;
    int32 ObjectPermutationMax = 0;
    // Array of structs denoting the appearing objects for the respective section
    UPROPERTY(EditAnywhere, Category="ObjectPermutation")
    TArray<FObjectApperanceStruct> ObjectPermutation;

    UPROPERTY(EditAnywhere, Category="EnvironmentPermutation")
    int32 StartEnvironmentComplexity = 0;
    int32 EnvironmentComplexityMax;
    // Next EnvironmentComplextiyIncrease in Seconds
    UPROPERTY(EditAnywhere, Category="EnvironmentPermutation", meta=(ClampMin="1.0", UIMin="1.0"))
    int32 NextEnviromnetComplexityIncrease = 1;
    // Array of structs denoting the enivonment setup for the respective section
    UPROPERTY(EditAnywhere, Category="EnvironmentPermutation")
    TArray<FEnvironmentStruct> EnvironmentSettings;

    UPROPERTY(EditAnywhere, Category="MaterialPermutation")
    int32 StartMaterialComplexity = 0;
    // Next MaterialComplextiy increase in Seconds
    UPROPERTY(EditAnywhere, Category="MaterialPermutation", meta=(ClampMin="1.0", UIMin="1.0"))
    int32 NextMaterialComplexityIncrease = 1;
    // Array of structs denoting the shader setup for the respective section
    UPROPERTY(EditAnywhere, Category="MaterialPermutation")
    TArray<FMaterialActivationStruct> ShaderComplexity;
};
