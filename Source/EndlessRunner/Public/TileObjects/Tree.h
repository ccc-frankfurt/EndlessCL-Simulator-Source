// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Math/Color.h"

#include "CoreMinimal.h"
#include "TileObjects/BaseObject.h"
#include "Tree.generated.h"

/**
 * 
 */
UCLASS()
class ENDLESSRUNNER_API ATree : public ABaseObject
{
	GENERATED_BODY()
	
public:
    ATree();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Tree")
    FLinearColor LeafColorDark = FLinearColor::Black;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Tree")
    FLinearColor LeafColorBright = FLinearColor::Black;

    bool bUseLineTraceSpawn = false;


protected:
    void OnConstruction(const FTransform &transform);
    void CheckLocationForCollsion(FHitResult& HitResult);

    void GetRandomLinearColor();
};
