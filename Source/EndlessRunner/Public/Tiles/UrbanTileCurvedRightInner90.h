// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tiles/UrbanTile.h"
#include "UrbanTileCurvedRightInner90.generated.h"

/**
 * 
 */
UCLASS()
class ENDLESSRUNNER_API AUrbanTileCurvedRightInner90 : public AUrbanTile
{
	GENERATED_BODY()
	
public:
    AUrbanTileCurvedRightInner90();

protected:
    virtual void OnConstruction(const FTransform &transform) override;
    virtual void BeginPlay() override;
private:
};
