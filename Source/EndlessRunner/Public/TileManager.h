// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

class AComplexityManager;
class ARandomnessManager;
class AObjectFactoryBase;

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Tiles/BaseTile.h"
#include <Engine/World.h>
#include "Common/CommonStructs.h"
#include "EndlessRunnerCharacter.h"
#include "TileManager.generated.h"

UCLASS()
class ENDLESSRUNNER_API ATileManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATileManager();

    UWorld* WorldPtr;

    // Size of tile ringbuffer
    UPROPERTY(EditAnywhere, Category="TileManager")
    uint32 TileBufferSize = 7;

    // Depricated!
    //UPROPERTY(EditAnywhere, Category="TileManager")
    int32 TilesUntilReset = 4;

    // Number of tiles to pass in one sequence
    //UPROPERTY(EditAnywhere, Category="TileManager")
    int32 TilesToBePassedInSquence = 42;

    // Mean number of tiles passing until a new curve is passed
    UPROPERTY(EditAnywhere, Category="TileManager")
    float CurveTilesToSpawnMean = 4.f;
    // Deviation from the mean number of tiles till curve
    UPROPERTY(EditAnywhere, Category="TileManager")
    float CurveTilesToSpawnStddev = 0.45f;

    // Map of object names to object sample paths
    UPROPERTY(EditAnywhere, Category="TileObjects")
    TMap<FString, FString> TileObjectSampleDirectoryMap = TMap<FString, FString>();

    // Map of object names to sampling count
    UPROPERTY(EditAnywhere, Category="TileObjects")
    TMap<FString, FMinMax> TileObjectSamplingNumbers = TMap<FString, FMinMax>();

    FTransform MainStreetNextAnchor = FTransform();

    FString* BaseMeshPath;
    FString* BaseMaterialPath;

    // Complexity Manager
    int32 ObjectComplexity = 0;
    int32 MaterialCompexity = 0;

    // ObjectFactory
    AObjectFactoryBase* ObjectFactory = nullptr;

private:
    AEndlessRunnerCharacter* Charakter;

    FVector MainStreeStartLocation = FVector(-200000.f,0.f,0.f);
    FVector MainStreetEndLocation = FVector(200000.f,0.f, 0.f);

    uint32 TileBufferIndex = 0;
    TArray<ABaseTile*> TileRingBuffer = TArray<ABaseTile*>();

    int32 TilesPassed = 0; // should be renamed TilesSpawned

    int32 TilesTillCurve = 5;
    bool IsLastTileCurve = false;

    bool IsNewSubSequence = true;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

    void RegisterToGameMode();

    void InitTileBuffer(uint32 NumTiles);

    void SetTilesToBePassedInSequence(FSequence SequenceSetup);

    void UpdateTilesTillCurve();
    
    ABaseTile* SpawnCrossing(FTransform &Transform);
    AComplexityManager* GetComplexityManager();
    ARandomnessManager* GetRandomnessManager();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

    void SetMainStreetNextAnchor(FTransform NextTransform);
    ABaseTile* SpawnMainStreetTile();

    FMinMax GetObjectSamplingRange(FString ObjectClassName);

    void LoadSequenceModule(FSequence ActiveSequence);
    void LoadSubSequenceModule(FSubSequence ActiveSubSequence);

    void InitialTileSpawn();
    void IncreaseTilesPassed();

    bool CheckIfSpawnActor(FString ActorName);

    bool CheckIfResetTileLocation();
    bool CheckIfSequencFinished();
    void MoveTilesToStartLocation();
    void ResetMainStreetNextAnchor();
    void MoveCharkaterToStartLocation();

    uint32 GetTileBufferSize();

private:
    void AddTileToRingBuffer(ABaseTile* NewTile);
    void MonitorTileOutOfRange();
};
