// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tiles/BaseTile.h"
#include "GameFramework/Actor.h"
#include "TerrainTile.generated.h"

UCLASS()
class ENDLESSRUNNER_API ATerrainTile : public ABaseTile
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATerrainTile();

    UArrowComponent* NextSpawnArrow;

protected:

    virtual void OnConstruction(const FTransform& transform) override;
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
    virtual void Init() override;

    void SpawnAdditionalObjects();

private:
    void SpawnBuilding(const FTransform &transform);
    void SpawnTrees(const FTransform &transform);

};
