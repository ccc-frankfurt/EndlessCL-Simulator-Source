// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tiles/SideWalkTile.h"
#include "SideWalkTileCurvedRightOuter90.generated.h"

/**
 * 
 */
UCLASS()
class ENDLESSRUNNER_API ASideWalkTileCurvedRightOuter90 : public ASideWalkTile
{
	GENERATED_BODY()

public:
    ASideWalkTileCurvedRightOuter90();

protected:
    virtual void OnConstruction(const FTransform &transform) override;
    virtual void BeginPlay() override;

    void SpawnOuterTerrain(const FTransform &transform);

    void SpawnAdditionalObjects();

private:
    void SpawnLampPoles();
	
};
