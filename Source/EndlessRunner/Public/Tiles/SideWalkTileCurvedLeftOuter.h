// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

class USplineComponent;

#include "CoreMinimal.h"
#include "Tiles/SideWalkTile.h"
#include "SideWalkTileCurvedLeftOuter.generated.h"

/**
 * 
 */
UCLASS()
class ENDLESSRUNNER_API ASideWalkTileCurvedLeftOuter : public ASideWalkTile
{
	GENERATED_BODY()
	
public:
    ASideWalkTileCurvedLeftOuter();

    void SpawnOuterTerrain(const FTransform &transform);

protected:
    virtual void OnConstruction(const FTransform& transform) override;
    virtual void BeginPlay() override;

    void SpawnAdditionalObjects();

private:
    void SpawnLampPoles();
};
