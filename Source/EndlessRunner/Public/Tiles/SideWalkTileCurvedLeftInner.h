// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tiles/SideWalkTile.h"
#include "SideWalkTileCurvedLeftInner.generated.h"

/**
 * 
 */
UCLASS()
class ENDLESSRUNNER_API ASideWalkTileCurvedLeftInner : public ASideWalkTile
{
    GENERATED_BODY()

public:
    ASideWalkTileCurvedLeftInner();

    void SpawnTerrainInner(const FTransform& transform);

protected:
    virtual void OnConstruction(const FTransform &transform) override;
    virtual void BeginPlay() override;

    void SpawnAdditionalObjects();

private:
    void SpawnLampPoles();
};
