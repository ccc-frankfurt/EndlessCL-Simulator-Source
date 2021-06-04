// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "EndlessRunnerGameMode.h"
#include "EndlessRunnerCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "EngineUtils.h"
#include "Engine/World.h"

#include "RandomnessManager.h"
#include "TileManager.h"
#include "SequenceManager.h"
#include "EnvironmentManager.h"
#include "SegmentationManager.h"
#include "ComplexityManager.h"

#include "GenericPlatform/GenericPlatformMisc.h"
#include "Kismet/GameplayStatics.h"

AEndlessRunnerGameMode::AEndlessRunnerGameMode()
{
    // set default pawn class to our Blueprinted character
    /*
    static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
    if (PlayerPawnBPClass.Class != NULL)
    {
        DefaultPawnClass = PlayerPawnBPClass.Class;
    }
    */
    // Set no DefaultPawn class as it is not needed
    DefaultPawnClass = nullptr; 
}

void AEndlessRunnerGameMode::BeginPlay(){
    Super::BeginPlay();
    // Get WorldPtr
    UWorld* worldPtr = GetWorld();

    UE_LOG(LogWindows, Warning, TEXT("EndlessRunnerGameMode began Play!"));

    // Get Reference to RandomManager (needs to gathered first to be available to all other managers)
    for (TActorIterator<ARandomnessManager> ActorItr(worldPtr, ARandomnessManager::StaticClass()); ActorItr; ++ActorItr){
        // Assuming there is only one ComplexityManager
        RandomnessManager = *ActorItr;
        UE_LOG(LogTemp, Warning, TEXT("found RandomnessManager"));
    }
    if(!RandomnessManager){
        UE_LOG(LogTemp, Error, TEXT("No RandomnessManager found!"));
    }

    // Get Reference to ComplexityManager
    for (TActorIterator<AComplexityManager> ActorItr(worldPtr, AComplexityManager::StaticClass()); ActorItr; ++ActorItr){
        // Assuming there is only one ComplexityManager
        ComplexityManager = *ActorItr;
        UE_LOG(LogTemp, Warning, TEXT("found ComplexityManager"));
    }
    if(!ComplexityManager){
        UE_LOG(LogTemp, Error, TEXT("No ComplexityManager found!"));
    }

    // Get Reference to EnvironmentManager
    for (TActorIterator<AEnvironmentManager> ActorItr(worldPtr, AEnvironmentManager::StaticClass()); ActorItr; ++ActorItr){
        // Assuming there is only one EnvironmentManager
        EnvironmentManager = *ActorItr;
        UE_LOG(LogTemp, Warning, TEXT("found EnvironmentManager"));
        // Set Connection to ComplexityManager
        if(ComplexityManager){
            ComplexityManager->SetEnvironmentManagerCallback(EnvironmentManager);
        }
    }
    if(!ComplexityManager){
        UE_LOG(LogTemp, Error, TEXT("No EnvironmentManager found!"));
    }

    // Get Reference to SegmentationManager
    for (TActorIterator<ASegmentationManager> ActorItr(worldPtr, ASegmentationManager::StaticClass()); ActorItr; ++ActorItr){
        // Assuming there is only one SegmentationManager
        SegmentationManager = *ActorItr;
        UE_LOG(LogTemp, Warning, TEXT("found SegmentationManager"));
        // Set Connection to ComplextiyManager
        //if(ComplexityManager){
        //    ComplexityManager->SetSegmentationManagerCallback(SegmentationManager);
        //}
    }
    if(!SegmentationManager){
        UE_LOG(LogTemp, Error, TEXT("No SegmentationManager found!"));
    }

    // Get Reference to TileManager
    for (TActorIterator<ATileManager> ActorItr(worldPtr, ATileManager::StaticClass()); ActorItr; ++ActorItr){
        // Assuming there is only one SegmentationManager
        TileManager = *ActorItr;
        UE_LOG(LogTemp, Warning, TEXT("found TileManager"));
    }
    if(!SegmentationManager){
        UE_LOG(LogTemp, Error, TEXT("No TileManager found!"));
    }

    // Get Reference for SequenceManager
    for(TActorIterator<ASequenceManager> ActorItr(worldPtr, ASequenceManager::StaticClass()); ActorItr; ++ActorItr){
        // Assuming there is only one SequenceManager
        SequenceManager = *ActorItr;
        UE_LOG(LogTemp, Warning, TEXT("found SequenceManager"));
    }
    if(!SequenceManager){
        UE_LOG(LogTemp, Error, TEXT("No SequenceManager found!"));
    }

    // Initial Spawn of Tiles
    //TileManager->InitialTileSpawn();

}

APawn* AEndlessRunnerGameMode::SpawnDefaultPawnFor(AController *Controller, AActor* StartSpot){
    //Overriding function to return nullptr which stops DefaultPawn from being spawned
    return nullptr;
}


ASegmentationManager* AEndlessRunnerGameMode::FindSegmentationManager(){
    // Get Reference to SegmentationManager
    for (TActorIterator<ASegmentationManager> ActorItr(GetWorld(), ASegmentationManager::StaticClass()); ActorItr; ++ActorItr){
        // Assuming there is only one SegmentationManager
        ASegmentationManager *segmentationManager = *ActorItr;
        UE_LOG(LogTemp, Warning, TEXT("GameMode: Found SegmentationManager"));
        return segmentationManager;
    }
    UE_LOG(LogTemp, Error, TEXT("GameMode: No SegmentationManager found!"));
    return nullptr;
}

AEnvironmentManager* AEndlessRunnerGameMode::FindEnvironmentManager(){
    for (TActorIterator<AEnvironmentManager> ActorItr(GetWorld(), AEnvironmentManager::StaticClass()); ActorItr; ++ActorItr){
        // Assuming there is only one SegmentationManager
        AEnvironmentManager *environmentManager = *ActorItr;
        UE_LOG(LogTemp, Warning, TEXT("GameMode: Found SegmentationManager"));
        return environmentManager;
    }
    UE_LOG(LogTemp, Error, TEXT("GameMode: No EnvironmentManager found!"));
    return nullptr;
}

ARandomnessManager* AEndlessRunnerGameMode::FindRandomnessManager(){
    for (TActorIterator<ARandomnessManager> ActorItr(GetWorld(), ARandomnessManager::StaticClass()); ActorItr; ++ActorItr){
        // Assuming there is only one SegmentationManager
        ARandomnessManager *randomnessManager = *ActorItr;
        UE_LOG(LogTemp, Warning, TEXT("GameMode: Found RandomnessManager"));
        return randomnessManager;
    }
    UE_LOG(LogTemp, Error, TEXT("GameMode: No RandomnessManager found!"));
    return nullptr;
}

ATileManager* AEndlessRunnerGameMode::FindTileManager(){
    for (TActorIterator<ATileManager> ActorItr(GetWorld(), ATileManager::StaticClass()); ActorItr; ++ActorItr){
        // Assuming there is only one SegmentationManager
        ATileManager *tileManager = *ActorItr;
        UE_LOG(LogTemp, Warning, TEXT("GameMode: Found TileManager"));
        return tileManager;
    }
    UE_LOG(LogTemp, Error, TEXT("GameMode: No TileManager found!"));
    return nullptr;
}

void AEndlessRunnerGameMode::QuitGame(){
    UE_LOG(LogTemp, Log, TEXT("QuitGame called..."));
    UE_LOG(LogTemp, Log, TEXT("Simulation Done!"));
    //FGenericPlatformMisc::RequestExit(false); // this quits the editor aswell
    GetWorld()->GetFirstPlayerController()->ConsoleCommand("quit");
}

void AEndlessRunnerGameMode::DestoryAllTiles(){
    for(TActorIterator<ABaseTile> ActorItr(GetWorld(), ABaseTile::StaticClass()); ActorItr; ++ActorItr){
        ABaseTile* tile = *ActorItr;
        UE_LOG(LogTemp, Error, TEXT("Destroying Tile: %s"), *tile->GetName());
        tile->Destroy();
    }
}

ARandomnessManager* AEndlessRunnerGameMode::GetRandomnessManager(){
    return RandomnessManager;
}

ATileManager* AEndlessRunnerGameMode::GetTileManager(){
    return TileManager;
}

AComplexityManager* AEndlessRunnerGameMode::GetComplexityManager(){
    return ComplexityManager;
}
