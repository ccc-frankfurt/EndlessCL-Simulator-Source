// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

class UArrowComponent;

#include "CoreMinimal.h"
#include "TileObjects/BaseObject.h"
#include "UrbanHouse.generated.h"

/**
 * 
 */
UCLASS()
class ENDLESSRUNNER_API AUrbanHouse : public ABaseObject
{
	GENERATED_BODY()
	
public:
    AUrbanHouse();

    float ForwardBoundX = 0.f;
    float BackwardBoundX = 0.f;


protected:
    void OnConstruction(const FTransform &Transform);


};
