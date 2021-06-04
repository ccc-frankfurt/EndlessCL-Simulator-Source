// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

class ARandomnessManager;
class AComplexityManager;
class AEnvironmentManager;
class ASegmentationManager;
class ATileManager;

#include "ManagerModules/BaseSequenceModule.h"
#include "ManagerModules/SequenceModule.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SequenceManager.generated.h"

UCLASS()
class ENDLESSRUNNER_API ASequenceManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASequenceManager();
    ~ASequenceManager();

    // Wethere to use the SequenceModule or read from a json file
    UPROPERTY(EditAnywhere, Category="SequenceSetups")
    bool ReadFromJson = false;

    UPROPERTY(EditAnywhere, Category="SequenceSetups")
    FString JsonFile = "";

    //UPROPERTY(EditAnywhere, Category="SequenceManager")
    //bool IsActive = false;

    UPROPERTY(EditAnywhere, Category="SequenceSetups")
    //TArray<UBaseSequenceModule*> SequenceModules;
    TArray<USequenceModule*> SequenceModules;

    int32 ActiveSequenceIndex = 0;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FSequence ReadSequencesFromJsonFile(FString JsonFilePath);

protected:
    FSequence ActiveSequenceSetup;

    TArray<FString> JsonSequenceFilePaths;

private:
    bool IsResetSequence = false;

    // Counter tracking global number of sequences done
    int32 SequenceCounter = 0;
    // Counter tracking repetitions of one module
    int32 SequenceCounterSpecific = 0;
    // String that will contain the JSON content of this sequence
    FString JSONSequenceString = "";


public:
    void AddToSequenceJSON(FEnvironmentSettingStruct SettingsStruct);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

    void RegisterToGameMode();

    void LoadSequenceData();
    void PropagateSequenceSetup();

    // Check for Module having multiple sequence runs
    void CheckNumSequenceIterations();

    void SetNextSequenceModule();

    void InitSequenceJSON();
    void SaveSequenceJSON();

    void WriteSegmentationJSON();

    AComplexityManager* GetComplexityManager();
    ARandomnessManager* GetRandomnessManager();
    AEnvironmentManager* GetEnvironmentManager();
    ASegmentationManager* GetSegmentationManager();
    ATileManager* GetTileManager();


	
};
