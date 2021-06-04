// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tiles/BaseTile.h"
#include "UrbanTile.generated.h"

/**
 * 
 */
UCLASS()
class ENDLESSRUNNER_API AUrbanTile : public ABaseTile
{
	GENERATED_BODY()
public:
    AUrbanTile();

    UArrowComponent* NextSpawnArrow;

    UArrowComponent* BuildingSpawnA;
    UArrowComponent* BuildingSpawnB;

protected:
    virtual void OnConstruction(const FTransform& transform) override;
    virtual void BeginPlay() override;

public:
    void SpawnAdditionalObjects();

private:
    void SpawnBuilding(UArrowComponent* Spawn);
	
};
