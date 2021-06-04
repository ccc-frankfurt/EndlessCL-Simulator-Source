// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

class UBoxCollision;

#include "CoreMinimal.h"
#include "Tiles/BaseTile.h"
#include "UrbanTCrossingCurveTile.generated.h"

/**
 * 
 */
UCLASS()
class ENDLESSRUNNER_API AUrbanTCrossingCurveTile : public ABaseTile
{
	GENERATED_BODY()
	
public:
    AUrbanTCrossingCurveTile();



protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BaseTile")
    ETileSides TileSide = ETileSides::NONE;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="UrbanTerrain")
    int32 NumBuildings;



protected:
    virtual void OnConstruction(const FTransform& Transform) override;
    virtual void BeginPlay() override;

private:
    void SpawnAdditionalObjects();
    void SpawnBuilding(float SplinePercentage, float MinSpawnOffset = 0.f, float MaxSpawnOffset = 0.f);


	
};
