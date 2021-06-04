// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tiles/UrbanTile.h"
#include "UrbanTileCurvedRightOuter90.generated.h"

/**
 * 
 */
UCLASS()
class ENDLESSRUNNER_API AUrbanTileCurvedRightOuter90 : public AUrbanTile
{
	GENERATED_BODY()
	
public:
    AUrbanTileCurvedRightOuter90();

protected:
    virtual void OnConstruction(const FTransform &transform) override;
    virtual void BeginPlay() override;

    void SpawnAdditionalObjects();

private:
    void SpawnBuilding(float SplinePercentage);
};
