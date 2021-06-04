// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tiles/SideWalkTile.h"
#include "SideWalkTCrossing.generated.h"

/**
 * 
 */
UCLASS()
class ENDLESSRUNNER_API ASideWalkTCrossing : public ASideWalkTile
{
	GENERATED_BODY()
	
public:
    ASideWalkTCrossing();

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BaseTile")
    ETileSides TileSide = ETileSides::NONE;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="TCrossingSideWalk")
    UClass* TerrainClass = nullptr;

protected:
    virtual void OnConstruction(const FTransform &transform) override;
    virtual void BeginPlay() override;

    void SpawnAdditionalObjects();

private:
    void SpawnTerrainByReference(FString AssetReference, const FTransform& Transform);
	
};
