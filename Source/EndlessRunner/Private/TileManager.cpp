// Fill out your copyright notice in the Description page of Project Settings.


#include "TileManager.h"

#include "ComplexityManager.h"
#include "RandomnessManager.h"

#include "Tiles/MainStreetTile.h"
#include "Tiles/MainStreetTile_Curved_Left.h"
#include "Tiles/MainStreetTileCurvedRight90.h"
#include "Tiles/TCrossing.h"

#include "TileObjects/ObjectFactory/ObjectFactoryBase.h"

#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Runtime/Engine/Public/EngineUtils.h"
#include "EndlessRunnerGameMode.h"

#include "Engine.h"


// Sets default values
ATileManager::ATileManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATileManager::BeginPlay()
{
	Super::BeginPlay();

    // Register to GameMode (needed when not being the root level)
    RegisterToGameMode();

    // Init World Pointer
    WorldPtr = GetWorld();

    // Init ObjectFactory
    ObjectFactory = WorldPtr->SpawnActor<AObjectFactoryBase>(AObjectFactoryBase::StaticClass(), FTransform());

    // Get Reference to Player Charakter
    Charakter = (AEndlessRunnerCharacter*)UGameplayStatics::GetPlayerCharacter(WorldPtr, 0);

    // Init TileRingBuffer
    InitTileBuffer(TileBufferSize);
}

// Called every frame
void ATileManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    // Check whethere tiles are spawned close to maximum world position
    MonitorTileOutOfRange();
}

void ATileManager::LoadSequenceModule(FSequence ActiveSequence){
    // Calculate the number of Tiles to be passed for all SubsSequences
    SetTilesToBePassedInSequence(ActiveSequence);
    // Re-Init the TileBuffer with new size
    TileBufferSize = ActiveSequence.NumberConcurrentTiles;
    InitTileBuffer(TileBufferSize);
}

void ATileManager::LoadSubSequenceModule(FSubSequence ActiveSubSequence){
    // Set TileObject SamplingNumbers
    TileObjectSamplingNumbers = ActiveSubSequence.ObjectSamplingNumbers;
    // Set StreetSampling
    CurveTilesToSpawnMean = ActiveSubSequence.StreetSampling.TilesToNextCurveMean;
    CurveTilesToSpawnStddev = ActiveSubSequence.StreetSampling.TilesToNextCurveStddev;
    UpdateTilesTillCurve();
}

void ATileManager::InitTileBuffer(uint32 NumTiles){
    // Emptry/Reset the TileBuffer
    TileRingBuffer.Empty();

    // Init TileRingBuffer
    TileRingBuffer.SetNumUninitialized(NumTiles, false);
    for(size_t i=0; i<TileRingBuffer.Num(); i++){
        TileRingBuffer[i] = nullptr;
    }
}

void ATileManager::SetTilesToBePassedInSequence(FSequence SequenceSetup){
    // Accumulate number of tiles for each SubSequence
    int32 tilesToBePasse = 0;
    for(FSubSequence subSequence : SequenceSetup.SubSequenceSettings){
        tilesToBePasse += subSequence.NumberOfTiles;
    }

    // Set TilesToBePassedInSquence
    TilesToBePassedInSquence = tilesToBePasse;
    UE_LOG(LogTemp, Warning, TEXT("Tiles to be passed for this sequence are: %d"), TilesToBePassedInSquence);
    return;
}


void ATileManager::MonitorTileOutOfRange(){
    float cirticalDistance = HALF_WORLD_MAX1 * 0.8f;
    if(FMath::Abs(MainStreetNextAnchor.GetLocation().X) > cirticalDistance
            || FMath::Abs(MainStreetNextAnchor.GetLocation().Y) > cirticalDistance){
        UE_LOG(LogTemp, Fatal, TEXT("TileManager: Tiles are spawning outside save area: You are getting close to the end of the world!"));
    }
}

void ATileManager::SetMainStreetNextAnchor(FTransform NextTransform){
    MainStreetNextAnchor = NextTransform;
}

ABaseTile* ATileManager::SpawnMainStreetTile(){
    // TODO: Have a modular function controlling the main street flow

    // Select a Tile type and spawn the Tile
    ABaseTile* newTile = nullptr;
    if((TilesPassed+1) % (TilesTillCurve) == 0 && !IsLastTileCurve){
        if(GetRandomnessManager()->GetFloat() >= 0.5f){ // Spawn Crossing
            UE_LOG(LogTemp, Error, TEXT("SPAWNING Crossing"));
            newTile = SpawnCrossing(MainStreetNextAnchor);
        } else{ // Spawn Curve
            if(GetRandomnessManager()->GetFloat() >= 0.5f){
                UE_LOG(LogTemp, Error, TEXT("SPAWNING CurveRight"));
                newTile = WorldPtr->SpawnActorDeferred<AMainStreetTileCurvedRight90>(
                            AMainStreetTileCurvedRight90::StaticClass(), MainStreetNextAnchor);
            } else{
                UE_LOG(LogTemp, Error, TEXT("SPAWNING CurveLeft"));
                newTile = WorldPtr->SpawnActorDeferred<AMainStreetTile_Curved_Left>(
                            AMainStreetTile_Curved_Left::StaticClass(), MainStreetNextAnchor);
            }
            // Update tiles to next curve spawn
            UpdateTilesTillCurve();
            IsLastTileCurve = true;
       }
    } else{
        UE_LOG(LogTemp, Error, TEXT("SPAWNING StraightStreet"));
        newTile = WorldPtr->SpawnActorDeferred<AMainStreetTile>(AMainStreetTile::StaticClass(), MainStreetNextAnchor);
        IsLastTileCurve = false;
    }
    UGameplayStatics::FinishSpawningActor(newTile, MainStreetNextAnchor);

    // Write current environment settings to tile
    GetComplexityManager()->SetEnvironmentSettings(newTile);

    // On complexity change the next spawned tile needs an active flag
    if(IsNewSubSequence){
        newTile->IsNewEnvironment = true;
        // Reset Flag
        IsNewSubSequence = false;
    }

    // Add new tile to this TileManagers ring buffer
    AddTileToRingBuffer(newTile);

    // FIXME: remove this return..
    return newTile;
}

ABaseTile* ATileManager::SpawnCrossing(FTransform& Transform){
    // TODO: Make reference finding more dynamic, like finding materials etc.
    FString TCrossingFrontalRef = "Class'/Game/Geometry/TCrossing/Frontal/TCrossingFrontal_BP.TCrossingFrontal_BP_C'";
    FString TCrossingRightRef = "Class'/Game/Geometry/TCrossing/Right/TCrossingRight_BP.TCrossingRight_BP_C'";
    FString TCrossingLeftRef = "Class'/Game/Geometry/TCrossing/Left/TCrossingLeft_BP.TCrossingLeft_BP_C'";
    // Choose Reference to load
    FString ref = "";
    float randFloat = GetRandomnessManager()->GetFloat();
    if(randFloat <= 0.33f){
        ref = TCrossingFrontalRef;
    } else if (randFloat <= 0.66f){
        ref = TCrossingRightRef;
    } else{
        ref = TCrossingLeftRef;
    }
    // Get reference to BP class
    UClass* bpClass = LoadObject<UClass>(this, *ref);
    ABaseTile* crossingActor = nullptr;
    if(bpClass){
        return GetWorld()->SpawnActorDeferred<ATCrossing>(bpClass, Transform);
    } else{
        UE_LOG(LogTemp, Error, TEXT("TileManager: SpawnCrossing: Failed Loading Class!"));
    }
    return crossingActor;
}

void ATileManager::AddTileToRingBuffer(ABaseTile *NewTile){
    //Check if old tile needs to be destroyed
    if(TileRingBuffer[TileBufferIndex]){
        TileRingBuffer[TileBufferIndex]->Destroy();
    }
    // Add new tile to the ring buffer
    TileRingBuffer[TileBufferIndex] = NewTile;
    // Update ring buffer index for next tile
    TileBufferIndex = (TileBufferIndex + 1) % TileBufferSize;
}

void ATileManager::IncreaseTilesPassed(){
    TilesPassed += 1; 
    UE_LOG(LogTemp, Warning, TEXT("TilesPassed: %d"), TilesPassed);

    // Check if complexity needs to be increased
    IsNewSubSequence = GetComplexityManager()->IsLoadNextSubSequence(TilesPassed);
    
    // Check if sequence is finished
    CheckIfSequencFinished();
    
    //TilesPassed += 1;
}


void ATileManager::InitialTileSpawn(){
    UE_LOG(LogTemp, Error, TEXT("Mean: %f, Stddev: %f, TTC: %d"), CurveTilesToSpawnMean, CurveTilesToSpawnStddev, TilesTillCurve);
    for(size_t i=0; i<TileBufferSize-1;i++){
        SpawnMainStreetTile();
        IncreaseTilesPassed(); // this is doing the inital offset to TilesPasse variable
    }
}

bool ATileManager::CheckIfSpawnActor(FString ActorName){
    return GetComplexityManager()->IsObjectSpawning(ActorName);
}

bool ATileManager::CheckIfResetTileLocation(){
    if((TilesPassed+1) % (TilesUntilReset+1) != 0){
        return false;
    }
    return true;
}

bool ATileManager::CheckIfSequencFinished(){
    if((TilesPassed - (TileBufferSize-1)) == TilesToBePassedInSquence){
        // TODO: Check if sequence or simulation ending

        // if simulation ending
        if(GetWorld()){
            if(GetWorld()->GetAuthGameMode()){
                AEndlessRunnerGameMode* GameModeCallback = (AEndlessRunnerGameMode*)GetWorld()->GetAuthGameMode();
                GameModeCallback->QuitGame();
                return false;
            }
        }
        return true;
    }
    else{
        // Add Debug Mesage
        GEngine->AddOnScreenDebugMessage(-1, 0.98f, FColor::Green, FString::Printf(TEXT("Tiles: %d, of %d"), (TilesPassed-TileBufferSize), TilesToBePassedInSquence));
    }
    return false;
}

FMinMax ATileManager::GetObjectSamplingRange(FString ObjectClassName){
    if(!TileObjectSamplingNumbers.Contains(ObjectClassName)){
        UE_LOG(LogTemp, Error, TEXT("TileManager: GetObjectSamplingRange: ObjectClassName is not known! Returning default range!"));
        return FMinMax();
    }
    return TileObjectSamplingNumbers[ObjectClassName];
}


void ATileManager::MoveCharkaterToStartLocation(){
    FVector targetOffset = FVector(0.1f,0.f,10.f);
    FVector targetLocation = Charakter->GetRootComponent()->GetComponentLocation();
    FRotator targetRotation = Charakter->GetRootComponent()->GetComponentRotation();
    targetLocation -= MainStreetNextAnchor.GetLocation();
    Charakter->GetRootComponent()->SetWorldLocationAndRotationNoPhysics(targetLocation, FRotator(0.f,0.f,0.f));
    //targetLocation += targetOffset;
    //Charakter->GetRootComponent()->SetWorldLocationAndRotationNoPhysics(targetLocation, FRotator(0.f,0.f,0.f));
    Charakter->TeleportTo(targetLocation, targetRotation);
}

void ATileManager::MoveTilesToStartLocation(){
    // Move all Tiles in RingBuffer
    for(ABaseTile* baseTilePointer : TileRingBuffer){
        if(baseTilePointer){
            // Move each tile by MainStreetStartLocation (interpreted as direction vector) to keep inter tile offsets as is
            baseTilePointer->ResetTileLocation(MainStreetNextAnchor.GetLocation());
        }
    }
    // Reset NextSpawnAnchor
    ResetMainStreetNextAnchor();
}

void ATileManager::ResetMainStreetNextAnchor(){
    // Get last spawned tile's index
    int32 currTileBufferIndex = 0;
    if(TileBufferIndex == 0){
        currTileBufferIndex = TileBufferSize-1; // last buffer element
    } else{
        currTileBufferIndex = TileBufferIndex-1;
    }
    // Get the Tile
    ABaseTile* tilePointer = TileRingBuffer[currTileBufferIndex];
    // Set MainStreetNextAnchor to this tiles spawn anchor
    if(tilePointer){
        SetMainStreetNextAnchor(tilePointer->SpawnForwardArrow->GetComponentTransform());
    }
}

uint32 ATileManager::GetTileBufferSize(){
    return TileBufferSize;
}

void ATileManager::UpdateTilesTillCurve(){
    TilesTillCurve = GetRandomnessManager()->SampleNormalDistributaion(CurveTilesToSpawnMean, CurveTilesToSpawnStddev);
    if(TilesTillCurve < 1){
        TilesTillCurve += 1;
    }
}

AComplexityManager* ATileManager::GetComplexityManager(){
    if(GetWorld()){
        if(GetWorld()->GetAuthGameMode()){
            AEndlessRunnerGameMode* GameModeCallback = (AEndlessRunnerGameMode*)GetWorld()->GetAuthGameMode();
            AComplexityManager* complexityManager = GameModeCallback->ComplexityManager;
            return complexityManager;
        }
    }
    return nullptr;
}

ARandomnessManager* ATileManager::GetRandomnessManager(){
    if(GetWorld()){
        if(GetWorld()->GetAuthGameMode()){
            AEndlessRunnerGameMode* GameModeCallback = (AEndlessRunnerGameMode*)GetWorld()->GetAuthGameMode();
            ARandomnessManager* randomnessManager = GameModeCallback->RandomnessManager;
            return randomnessManager;
        }
    }
    return nullptr;
}

void ATileManager::RegisterToGameMode(){
    if(GetWorld()){
        if(GetWorld()->GetAuthGameMode()){
            AEndlessRunnerGameMode* gameModeCallback = (AEndlessRunnerGameMode*)GetWorld()->GetAuthGameMode();
            gameModeCallback->TileManager = this;
            UE_LOG(LogTemp, Warning, TEXT("TileManager: Registered to GameMode"));
        }
    }
}