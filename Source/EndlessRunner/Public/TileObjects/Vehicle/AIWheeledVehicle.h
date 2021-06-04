// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

class UStaticMeshComponent;
class USphereComponent;

#include "CoreMinimal.h"
#include "TileObjects/Vehicle/WheeledVehicleBase.h"
#include "AIWheeledVehicle.generated.h"

/**
 * 
 */
UCLASS()
class ENDLESSRUNNER_API AAIWheeledVehicle : public AWheeledVehicleBase
{
	GENERATED_BODY()
	
public:
    AAIWheeledVehicle();

public:
    virtual void Tick(float DeltaTime) override;

protected:


protected:
    virtual void OnConstruction(const FTransform &transform) override;
    virtual void BeginPlay()override;

};
