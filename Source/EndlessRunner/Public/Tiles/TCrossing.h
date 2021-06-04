// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

class USplineComponent;
class UArrowComponent;
class UBoxComponent;

#include "CoreMinimal.h"
#include "Tiles/BaseTile.h"
#include "TCrossing.generated.h"

/**
 * 
 */
UCLASS()
class ENDLESSRUNNER_API ATCrossing : public ABaseTile
{
	GENERATED_BODY()
	
public:
    ATCrossing();

    virtual USplineComponent* GetMeshSpline(FVector ActorLocation) override;
    virtual USplineComponent* GetPlayerMeshSpline(FVector ActorLocation) override;
    virtual ETileSides GetMeshSplineName(FVector ActorLocation) override;
    virtual ETileSides GetPlayerMeshSplineName() override;
    USplineComponent* GetMeshSplineByName(ETileSides TileSideName) override;

    float TimeToNextTrafficSwitch = 5.f; // seconds
    float TimeSinceLastTrafficSwitch = 0.f; // seconds
    int32 TrafficState = 0; // stupid counter for traffic state // FIXME: proper system needed

    UFUNCTION()
    void OnBoxOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);


protected:
    virtual void OnConstruction(const FTransform &transform) override;
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    void SetNextSpawnArrow();
    void SetNextSpawnAnchor();

    void SpawnSideWalkTileByReference(FString AssetReferenceName, const FTransform& Transform);
    void SpawnSideWalkTileByClass(UClass* AssetClass, const FTransform& Transform);

    void SpawnDummySteet(const FTransform& SpawnTransform);

    void SetupRoadBlockCollision(UStaticMeshComponent* RoadBlock);
    void EnableRoadBlockCollision(UStaticMeshComponent* RoadBlock);
    void DisableRoadBlockCollision(UStaticMeshComponent* RoadBlock);

    void DoTrafficControl();

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BoxCollision")
    UBoxComponent* BoxCollision = nullptr;

protected:
    // Blueprint class of the sidewalk left to be spawned by this class
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="TCrossing")
    UClass* SideWalkLeft = nullptr;
    // Blueprint class of the sidewalk right to be spawned by this class
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="TCrossing")
    UClass* SideWalkRight = nullptr;
    // Blueprint class of the sidewalk top side to be spawned by this class
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="TCrossing")
    UClass* SideWalkTop = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="TCrossing")
    USplineComponent* LeftSpline = nullptr;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="TCrossing")
    USplineComponent* RightSpline = nullptr;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="TCrossing")
    USplineComponent* TopSplineLeft = nullptr;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="TCrossing")
    USplineComponent* TopSplineRight = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="TCrossing")
    UArrowComponent* LeftTileSpawnArrow = nullptr;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="TCrossing")
    UArrowComponent* RightTileSpawnArrow = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="TCrossing")
    UArrowComponent* LeftNextSpawnArrow = nullptr;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="TCrossing")
    UArrowComponent* RightNextSpawnArrow = nullptr;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="TCrossing")
    UArrowComponent* TopNextSpawnArrow = nullptr;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="TCrossing")
    UArrowComponent* BottomNextSpawnArrow = nullptr;

    UArrowComponent* DummyStreetSpawnArrow = nullptr; // Arrow Component giving spawn for DummyStreet parts

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="TCrossing")
    UStaticMeshComponent* RoadBlock0 = nullptr;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="TCrossing")
    UStaticMeshComponent* RoadBlock1 = nullptr;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="TCrossing")
    UStaticMeshComponent* RoadBlock2 = nullptr;

     bool HasBeenTriggered = false;
};
