// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tiles/BaseTile.h"
#include <Engine/World.h>
#include "TileManager.h"
#include <Components/BoxComponent.h>
#include "Components/ArrowComponent.h"
#include "MainStreetTile.generated.h"

UCLASS()
class ENDLESSRUNNER_API AMainStreetTile : public ABaseTile
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMainStreetTile();

    UBoxComponent* BoxCollision;

    UArrowComponent* LeftSpawnArrow;
    UArrowComponent* RightSpawnArrow;

    bool IsMainStreet = true;

protected:
    bool HasBeenTriggered = false;

protected:
	// Called when the game starts or when spawned
    virtual void OnConstruction(const FTransform& transform) override;
    virtual void BeginPlay() override;

    void SetNextSpawnAnchor();
    void SpawnAdditionalObjects();

    void SpawnCar(float SplinePercentage, bool IsOncoming);

    virtual void ResetChildHandle(FVector LocationOffset) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

    virtual void Init() override;

    void SpawnSideWalk(const FTransform& transform, UArrowComponent* spawnArrow);

    UFUNCTION()
    void OnBoxOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

};
