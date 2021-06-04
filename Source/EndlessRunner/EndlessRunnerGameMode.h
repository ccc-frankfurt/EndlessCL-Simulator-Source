// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

class ATileManager;
class ASegmentationManager;
class ARandomnessManager;
class ASequenceManager;
class AEnvironmentManager;
class AComplexityManager;

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
//#include "TileManager.h"
//#include "SegmentationManager.h"
//#include "ComplexityManager.h"
//#include "EnvironmentManager.h"
//#include "RandomnessManager.h"
//#include "SequenceManager.h"
#include "GameFramework/Controller.h"
#include "EndlessRunnerGameMode.generated.h"

UCLASS(minimalapi)
class AEndlessRunnerGameMode : public AGameModeBase
{
	GENERATED_BODY()


public:
    ASequenceManager* SequenceManager = nullptr;

    ATileManager* TileManager = nullptr;
    ASegmentationManager* SegmentationManager = nullptr;
    AComplexityManager* ComplexityManager = nullptr;
    AEnvironmentManager* EnvironmentManager = nullptr;

    ARandomnessManager* RandomnessManager = nullptr;

protected:
    virtual void BeginPlay() override;

    virtual APawn* SpawnDefaultPawnFor(AController *Controller, AActor* StartSpot);

    void GetReferenceToSegmentationManager();

public:
    AEndlessRunnerGameMode();

    void QuitGame();
    UFUNCTION(BlueprintCallable, Category="EndlessRunnerGameMode")
    void DestoryAllTiles();

    AEnvironmentManager* FindEnvironmentManager();
    ASegmentationManager* FindSegmentationManager();
    ARandomnessManager* FindRandomnessManager();
    ATileManager* FindTileManager();

    ARandomnessManager* GetRandomnessManager();
    ATileManager* GetTileManager();
    UFUNCTION(BlueprintCallable, Category="EndlessRunnerGameMode")
    AComplexityManager* GetComplexityManager();
};



