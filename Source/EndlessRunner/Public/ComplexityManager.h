// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

class AEnvironmentManager;
class ARandomnessManager;
class ASegmentationManager;
class ATileManager;

class ABaseTile;

#include "Math/Color.h"

#include "ManagerModules/BaseSequenceModule.h"

#include "Common/CommonStructs.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ComplexityManager.generated.h"


UENUM()
enum class ETerrainTypes : int32
{
    PARK,
    URBAN,
    SUBURBAN,
    NUM
};

UCLASS()
class ENDLESSRUNNER_API AComplexityManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AComplexityManager();

    int32 ObjectComplexity = 3;

    AEnvironmentManager* EnvironmentManagerCallback;
    //ASegmentationManager* SegmentationManagerCallback;
    ARandomnessManager* RandomnessManagerCallback;
    ATileManager* TileManagerCallback;

    // Index of the SubSequence currently active
    int32 SubSequenceIndex = 0;
    // Number of tiles until the next SubSequence is loaded
    int32 NextSubSequenceIncrease = 0;

    // Copy of the SequenceManagers active sequence // TODO: Is there a better way of doing this?
    FSequence ActiveSequence;

    // Currnet ObjectPermutation setup
    UPROPERTY(EditAnywhere, Category="ObjectPermutation")
    FObjectApperanceStruct ObjectPermutation;

    // Current MaterialActivationPermutation
    UPROPERTY(EditAnywhere, Category="MaterialPermutation")
    FMaterialActivationStruct MaterialActivationPermutation;

    // Current EnvironmentSettings
    UPROPERTY(EditAnywhere, Category="EnvironmentPermutation")
    FEnvironmentStruct EnvironmentSettings;

    //UPROPERTY(EditAnywhere, Category="TerrainTypes", meta=(ClampMin="0.0",ClampMax="1.0",UIMin="0.0",UIMax="1.0"))
    float ParkProbabiltiy = 0.05f;
    //UPROPERTY(EditAnywhere, Category="TerrainTypes", meta=(ClampMin="0.0",ClampMax="1.0",UIMin="0.0",UIMax="1.0"))
    float UrbanProbability = 0.95f;
    //UPROPERTY(EditAnywhere, Category="TerrainTypes", meta=(ClampMin="0.0",ClampMax="1.0",UIMin="0.0",UIMax="1.0"))
    float SubUrbanProbability = 0.f;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

    void RegisterToGameMode();

    int32 GetNextEnvironmentStructIndex();

    ARandomnessManager* GetRandomnessManager();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

    bool IsLoadNextSubSequence(int32 TilesPassed);
    void LoadSequenceModule(FSequence SequenceSetting);
    void LoadSubSequence();
    void SetInternalVariables(); // TODO: remove

    UFUNCTION(BlueprintCallable, Category="ComplexityManager")
    void AdjustDynamicMaterial(UMaterialInstanceDynamic *DynamicMaterial);
    void AdjustDynamicMaterial(UMaterialInstanceDynamic *DynamicMaterial, FMaterialActivationStruct MaterialActivation);
    void SetDynamicMaterialColor(UMaterialInstanceDynamic* DynamicMaterial, int32 index, TMap<int32, FLinearColor> &MinColorMap, TMap<int32, FLinearColor> &MaxColorMap);
    bool IsObjectSpawning(FString ObjectName);

    void SetEnvironmentManagerCallback(AEnvironmentManager* CallbackManager);
    //void SetSegmentationManagerCallback(ASegmentationManager* CallbackManager);

    ETerrainTypes GetTerrainType();

    void SetEnvironmentSettings(ABaseTile* tile);
};
