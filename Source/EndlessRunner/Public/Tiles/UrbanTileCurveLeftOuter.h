// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tiles/UrbanTile.h"
#include "UrbanTileCurveLeftOuter.generated.h"

/**
 * 
 */
UCLASS()
class ENDLESSRUNNER_API AUrbanTileCurveLeftOuter : public AUrbanTile
{
	GENERATED_BODY()
	
public:
    AUrbanTileCurveLeftOuter();

protected:
    virtual void OnConstruction(const FTransform& transform) override;
    virtual void BeginPlay() override;

    void SpawnAdditionalObjects();

private:
    void SpawnBuilding(float SplinePercentage);
};
