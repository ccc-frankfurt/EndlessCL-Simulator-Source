// Fill out your copyright notice in the Description page of Project Settings.


#include "UrbanTileCurveLeftOuter.h"

#include "Components/SplineComponent.h"

#include "TileManager.h"
#include "RandomnessManager.h"

#include "TileObjects/UrbanHouse.h"
#include "TileObjects/Building.h"
#include "TileObjects/ObjectFactory/ObjectFactoryBase.h"

#include "Engine/World.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"


AUrbanTileCurveLeftOuter::AUrbanTileCurveLeftOuter(){

    // Set Class Variables
    MeshSamplePath = "/Geometry/UrbanCurvedLeft/Outer/";

    // Load Mesh from disc
    LoadObjectFromDisc();

}

void AUrbanTileCurveLeftOuter::OnConstruction(const FTransform &transform){
    // Setup MeshSpline
    MeshSpline->SetLocationAtSplinePoint(0, FVector(0.f, TileWidth*0.2f, 0.f), ESplineCoordinateSpace::Local);
    MeshSpline->SetTangentsAtSplinePoint(0, FVector(0.f, 0.f, 0.f), FVector(TileWidth*1.8f, 0.f, 0.f), ESplineCoordinateSpace::Local);

    MeshSpline->SetLocationAtSplinePoint(1, FVector(TileWidth*0.8f, -TileLength*0.63f, 0.f), ESplineCoordinateSpace::Local);
    MeshSpline->SetTangentsAtSplinePoint(1, FVector(0.f, -TileWidth*1.8f, 0.f), FVector(0.f,0.f,0.f), ESplineCoordinateSpace::Local);

}

void AUrbanTileCurveLeftOuter::BeginPlay(){
    ABaseTile::BeginPlay();

    // Spawn Additional Objects
    SpawnAdditionalObjects();
}

void AUrbanTileCurveLeftOuter::SpawnAdditionalObjects(){
    ATileManager* tileManager = GetTileManager();
    if(tileManager){
        if(tileManager->CheckIfSpawnActor("Building")){
            SpawnBuilding(0.1f);
            SpawnBuilding(0.3f);
            SpawnBuilding(0.5f);
            SpawnBuilding(0.7f);
            SpawnBuilding(0.9f);
        }
    }
}

void AUrbanTileCurveLeftOuter::SpawnBuilding(float SplinePercentage){
    // Get right vector at that distance on spline
    FVector rightVector = GetSplineRightVectorAtDistancePercentage(MeshSpline, SplinePercentage);
    // Get rotation from spline right vector
    FRotator spawnRotation = (rightVector*-1).ToOrientationRotator();
    // Get location at that disctance
    FVector spawnLocation = GetSplineLocationAtDistancePercentage(MeshSpline, SplinePercentage);
    // Add offset to that location
    spawnLocation += (rightVector*-1)*GetRandomnessManager()->GetFloatInRange(500.f, 1000.f);
    // Assample spawnTransform
    FTransform spawnTransform = FTransform(
                spawnRotation,
                spawnLocation,
                FVector::OneVector);
    // Spawn Actor
    //UClass* buildingClass = GetTileManager()->ObjectFactory->GetAssetClass("Building");
    UClass* buildingClass = GetTileManager()->ObjectFactory->GetAssetClassCategorical("Building");
    if(!buildingClass){
        UE_LOG(LogTemp, Error, TEXT("UrbanTileCurvedLeftOuter: No building class found"));
        return;
    }
    ABuilding* newBuilding = GetWorld()->SpawnActorDeferred<ABuilding>(buildingClass, spawnTransform);
    UGameplayStatics::FinishSpawningActor(newBuilding, spawnTransform);

    ChildActorArray.Add(newBuilding);
    if(newBuilding->SpawnBP){
        ChildActorArray.Add(newBuilding->SubMesh);
    }
}
