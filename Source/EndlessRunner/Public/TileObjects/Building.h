// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

class UArrowComponent;

#include "CoreMinimal.h"
#include "TileObjects/BaseObject.h"
#include "Building.generated.h"

/**
 * 
 */
UCLASS()
class ENDLESSRUNNER_API ABuilding : public ABaseObject
{
	GENERATED_BODY()

public:
    ABuilding();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BuildingBase")
    UArrowComponent* DirectionVector = nullptr;

    // Given by spawn calling class
    FVector SpawnDirection = FVector(0.f);
    float MinForwardOffset = 0.f;
    float MaxForwardOffset = 0.f;


protected:
    void OnConstruction(const FTransform &transform);

    void BeginPlay();
};
