// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tiles/UrbanTile.h"
#include "UrbanTileCurvedLeftInner.generated.h"

/**
 * 
 */
UCLASS()
class ENDLESSRUNNER_API AUrbanTileCurvedLeftInner : public AUrbanTile
{
	GENERATED_BODY()
	
public:
    AUrbanTileCurvedLeftInner();

protected:
    virtual void OnConstruction(const FTransform& transform) override;
    virtual void BeginPlay() override;
	
};
