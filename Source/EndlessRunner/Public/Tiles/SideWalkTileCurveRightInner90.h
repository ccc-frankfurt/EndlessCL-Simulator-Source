// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tiles/SideWalkTile.h"
#include "SideWalkTileCurveRightInner90.generated.h"

/**
 * 
 */
UCLASS()
class ENDLESSRUNNER_API ASideWalkTileCurveRightInner90 : public ASideWalkTile
{
	GENERATED_BODY()
	
	
public:
    ASideWalkTileCurveRightInner90();

protected:
    virtual void OnConstruction(const FTransform &transform) override;
    virtual void BeginPlay() override;

    void SpawnTerrainInner(const FTransform& transform);
    void SpawnAdditionalObjects();
private:
    void SpawnLampPoles();
};
