// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tiles/MainStreetTile.h"
#include "MainStreetTileCurvedRight90.generated.h"

/**
 * 
 */
UCLASS()
class ENDLESSRUNNER_API AMainStreetTileCurvedRight90 : public AMainStreetTile
{
	GENERATED_BODY()
	
public:
    AMainStreetTileCurvedRight90();

protected:
    virtual void OnConstruction(const FTransform& transfrom) override;
    virtual void BeginPlay();

    void SpawnAdditionalObjects();

    void SpawnCar(float SplinePercentage, bool IsOncoming);

    void SpawnSideWalkOuter(const FTransform& transform);
    void SpawnSideWalkInner(const FTransform& transform);
};
