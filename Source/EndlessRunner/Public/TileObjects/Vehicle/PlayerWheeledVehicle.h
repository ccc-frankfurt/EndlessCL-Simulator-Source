// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

class UCameraComponent;

#include "CoreMinimal.h"
#include "TileObjects/Vehicle/WheeledVehicleBase.h"
#include "PlayerWheeledVehicle.generated.h"

/**
 * 
 */
UCLASS()
class ENDLESSRUNNER_API APlayerWheeledVehicle : public AWheeledVehicleBase
{
	GENERATED_BODY()
	
	
public:
    APlayerWheeledVehicle();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="PlayerWheeledVehicle")
    UCameraComponent* PlayerCamera = nullptr;

protected:
    virtual void Tick(float DeltaTime) override;
    virtual void BeginPlay() override;
	
    virtual USplineComponent* GetNextGuideSpline(ABaseTile *tilePtr, FVector actorLocation) override;
    virtual ETileSides GetNextGuideSplineName(ABaseTile *tilePtr, FVector actorLocation) override;

    UFUNCTION()
    void WhenDestroyed(AActor* Actor);

protected:
    bool IsSelfDriving = true;
};
