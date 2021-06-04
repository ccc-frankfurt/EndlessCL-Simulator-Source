// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

class ATileManager;
class ASegmentationManager;
class AComplexityManager;
class ARandomnessManager;

class ABaseTile;

class USplineComponent;
class UArrowComponent;

class UCurveFloat;

#include "Tiles/BaseTile.h"

#include "Math/Color.h"
#include "Common/CommonStructs.h"

#include "CoreMinimal.h"
#include "WheeledVehicle.h"
#include "WheeledVehicleBase.generated.h"

/**
 * 
 */
UCLASS()
class ENDLESSRUNNER_API AWheeledVehicleBase : public AWheeledVehicle
{
	GENERATED_BODY()
	
public:
    AWheeledVehicleBase();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="WheeledVehicleBase")
    UArrowComponent* ProbeArrow = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="WheeledVehicleBase")
    UCurveFloat* BrakeCurve = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="WheeledVehicleBase")
    TMap<int32, FLinearColor> MaterialMinColorMap;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="WheeledVehicleBase")
    TMap<int32, FLinearColor> MaterialMaxColorMap;

    bool IsOncoming = false;
    UPROPERTY()
    USplineComponent* GuideSpline = nullptr;
    
    FString LastGuideSplineName = "";

    float LookAHeadSteeringDistance = 500.f;

    // DEBUG
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Debug")
    UStaticMeshComponent* DebugSphere = nullptr;

protected:
    FMaterialActivationStruct CurrSelfMaterialActivation;
    FMaterialActivationStruct CurrTileMaterialActivation;

    bool IsVisibileOnTile = true;

    float MaxSpeed = 20.f; // in Km/h
    float MaxSteeringAngle = 70.f; // in Degree

    float MaxThrottle = 0.6f;
    float MaxThrottelDampening = 0.7f;
    float CurrThrottel = 0.f;

    float CurrSteering = 0.f;
    float CurrDistanceToObstacle = 10000.f;
    float LastDistanceToObstacle = 10000.f;

    int32 FramesToNextLineTrace = 10;
    int32 NumFramesTillTrace = 0;

    int32 MaxNumEmptyLineTraces = 10;
    int32 NumEmptyLineTraces = 0;

    float MaxForwardTraceDistance = 650.f;

    ABaseTile* TileProbedLast = nullptr;

private:

public:
    virtual void Tick(float DeltaTime) override;

protected:
    virtual void OnConstruction(const FTransform &transform) override;
    virtual void BeginPlay() override;

    virtual USplineComponent* GetNextGuideSpline(ABaseTile *tilePtr, FVector actorLocation);
    virtual ETileSides GetNextGuideSplineName(ABaseTile* tilePtr, FVector actorLocation);

    void SetupMaterial();
    void SetupMaterial(FMaterialActivationStruct MaterialActivationPermutation);
	
    ASegmentationManager* GetSegmentationManager();
    AComplexityManager* GetComplexityManager();
    ARandomnessManager* GetRandomnessManager();

    void SetSegmentationIndex(FString ObjName);

    FMaterialActivationStruct GetMaterialActivationFromTile(ABaseTile* tilePtr);
    bool GetCarVisibilityFromTile(ABaseTile* tilePtr);

    bool CheckGuideSpline();

    void DoLineTrace();
    void DoForwardLineTrace();

    void FollowGuideSpline();
    void AdjustThrottle();

    float FindDistanceAlongSpline(FVector WorldLocation);

    void UpdateGuideSpline(USplineComponent* Spline);

    void DestroyOnEmptyLineTraces();

    float GetCurrSteering();
    float GetMaxSpeed();
};
