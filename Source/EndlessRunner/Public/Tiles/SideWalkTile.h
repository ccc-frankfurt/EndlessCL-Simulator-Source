// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tiles/BaseTile.h"
#include "GameFramework/Actor.h"
#include "SideWalkTile.generated.h"

UCLASS()
class ENDLESSRUNNER_API ASideWalkTile : public ABaseTile
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASideWalkTile();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SideWalkTile")
    UArrowComponent* NextSpawnArrow; //Spawn anchor for TerrainTile

    FIMinMax TreeOffsetY;

protected:
	// Called when the game starts or when spawned
    virtual void OnConstruction(const FTransform& transform) override;
	virtual void BeginPlay() override;

    virtual void ResetChildHandle(FVector LocationOffset) override;

    void SpawnLampPoles(const FTransform &transform);
    void SpawnTrees(const FTransform &transform, int32 NumMaxTrees=2);

    void SpawnPeople();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

    virtual void Init() override;

    void SpawnTerrain(const FTransform& transform, UArrowComponent* spawnArrow);
    void SpawnUrban(const FTransform& transform, UArrowComponent* spawnArrow);
    virtual void SpawnAdditionalObjects(); //FIXME: Move to BaseTile
    //void SpawnAdditionalObjectsMoving();

private:

};
