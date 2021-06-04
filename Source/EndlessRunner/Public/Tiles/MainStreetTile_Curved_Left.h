// Fill out your copyright notice in the Description page of Project Settings.

#pragma once



#include "CoreMinimal.h"
#include "Tiles/MainStreetTile.h"
#include "Components/SplineComponent.h"
#include "MainStreetTile_Curved_Left.generated.h"

/**
 * 
 */
UCLASS()
class ENDLESSRUNNER_API AMainStreetTile_Curved_Left : public AMainStreetTile
{
	GENERATED_BODY()
	
    AMainStreetTile_Curved_Left();

public:
    virtual void OnConstruction(const FTransform &transform) override;

protected:
    virtual void BeginPlay() override;

    void SpawnAdditionalObjects();

    void SpawnCar(float SplinePercentage, bool IsOncoming);

    void SpawnSideWalkInner(const FTransform& transform);
    void SpawnSideWalkOuter(const FTransform& transform);

};
