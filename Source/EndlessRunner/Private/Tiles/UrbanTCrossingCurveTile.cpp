// Fill out your copyright notice in the Description page of Project Settings.


#include "UrbanTCrossingCurveTile.h"

#include "Kismet/GameplayStatics.h"

#include "RandomnessManager.h"
#include "TileManager.h"

#include "TileObjects/ObjectFactory/ObjectFactoryBase.h"
#include "TileObjects/Building.h"


AUrbanTCrossingCurveTile::AUrbanTCrossingCurveTile(){
    return;
}

void AUrbanTCrossingCurveTile::OnConstruction(const FTransform &Transform){

    // Spawn Additional Objects
    SpawnAdditionalObjects();
}

void AUrbanTCrossingCurveTile::BeginPlay(){
    Super::BeginPlay();

}

void AUrbanTCrossingCurveTile::SpawnAdditionalObjects(){
    ATileManager* tileManager = GetTileManager();
    if(tileManager){
        if(tileManager->CheckIfSpawnActor("Building")){
            if(NumBuildings == 1){
                SpawnBuilding(0.f);
            } else if(NumBuildings == 5){
                SpawnBuilding(0.1f, 500.f, 1000.f);
                SpawnBuilding(0.3f, 500.f, 1000.f);
                SpawnBuilding(0.5f, 500.f, 1000.f);
                SpawnBuilding(0.7f, 500.f, 1000.f);
                SpawnBuilding(0.9f, 500.f, 1000.f);
            }
        }
    }
}

void AUrbanTCrossingCurveTile::SpawnBuilding(float SplinePercentage, float MinSpawnOffset, float MaxSpawnOffset){
    // Get right vector at that distance on spline
    FVector rightVector = GetSplineRightVectorAtDistancePercentage(MeshSpline, SplinePercentage);
    // Get rotation from spline right vector
    FRotator spawnRotation = rightVector.ToOrientationRotator();
    //spawnRotation.Add(0.f, 90.f, 0.f); // magic rotation for my buildings.. FIXME: figure out why this is needed
    // Get location at that disctance
    FVector spawnLocation = GetSplineLocationAtDistancePercentage(MeshSpline, SplinePercentage);
    // Add offset to that location
    spawnLocation += rightVector*GetRandomnessManager()->GetFloatInRange(MinSpawnOffset, MaxSpawnOffset);
    // Assample spawnTransform
    FTransform spawnTransform = FTransform(
                spawnRotation,
                spawnLocation,
                FVector::OneVector);
    // Spawn Actor
    UClass* buildingClass = GetTileManager()->ObjectFactory->GetAssetClassCategorical("Building");
    if(!buildingClass){
        UE_LOG(LogTemp, Error, TEXT("UrbanTCrossingCurveTile: No building class found"));
        return;
    }
    ABuilding* newBuilding = GetWorld()->SpawnActorDeferred<ABuilding>(buildingClass, spawnTransform);
    UGameplayStatics::FinishSpawningActor(newBuilding, spawnTransform);

    ChildActorArray.Add(newBuilding);
    if(newBuilding->SpawnBP){
        ChildActorArray.Add(newBuilding->SubMesh);
    }
}
