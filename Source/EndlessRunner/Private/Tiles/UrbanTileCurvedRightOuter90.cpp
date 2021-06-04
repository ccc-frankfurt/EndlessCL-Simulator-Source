// Fill out your copyright notice in the Description page of Project Settings.


#include "UrbanTileCurvedRightOuter90.h"

#include "Components/StaticMeshComponent.h"
#include "Components/SplineComponent.h"

#include "TileObjects/UrbanHouse.h"
#include "TileObjects/Building.h"

#include "TileManager.h"
#include "RandomnessManager.h"
#include "TileObjects/ObjectFactory/ObjectFactoryBase.h"
 	
#include "Kismet/GameplayStatics.h"


AUrbanTileCurvedRightOuter90::AUrbanTileCurvedRightOuter90(){
    // Set Class Varibales
    MeshSamplePath = "/Geometry/UrbanCurvedRight90/Outer/";

    // Load Mesh from disc
    LoadObjectFromDisc();
}

void AUrbanTileCurvedRightOuter90::OnConstruction(const FTransform &transform){
    // Setup MeshSpline
    MeshSpline->SetLocationAtSplinePoint(0, FVector(0.f, -TileWidth*0.2f, 0.f), ESplineCoordinateSpace::Local);
    MeshSpline->SetTangentsAtSplinePoint(0, FVector(0.f, 0.f, 0.f), FVector(TileWidth*1.8f, 0.f, 0.f), ESplineCoordinateSpace::Local);

    MeshSpline->SetLocationAtSplinePoint(1, FVector(TileWidth*0.8f, TileLength*0.63f, 0.f), ESplineCoordinateSpace::Local);
    MeshSpline->SetTangentsAtSplinePoint(1, FVector(0.f, TileWidth*1.8f, 0.f), FVector(0.f,0.f,0.f), ESplineCoordinateSpace::Local);

}

void AUrbanTileCurvedRightOuter90::BeginPlay(){
    ABaseTile::BeginPlay();

    // Spawn Additional Objects
    SpawnAdditionalObjects();
}


void AUrbanTileCurvedRightOuter90::SpawnAdditionalObjects(){
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

void AUrbanTileCurvedRightOuter90::SpawnBuilding(float SplinePercentage){
    // Get right vector at that distance on spline
    FVector rightVector = GetSplineRightVectorAtDistancePercentage(MeshSpline, SplinePercentage);
    // Get rotation from spline right vector
    FRotator spawnRotation = rightVector.ToOrientationRotator();
    // Get location at that disctance
    FVector spawnLocation = GetSplineLocationAtDistancePercentage(MeshSpline, SplinePercentage);
    // Add offset to that location
    spawnLocation += rightVector * GetRandomnessManager()->GetFloatInRange(500.f, 1000.f);
    // Assample spawnTransform
    FTransform spawnTransform = FTransform(
                spawnRotation,
                spawnLocation,
                FVector::OneVector);
    // Spawn Actor
    //UClass* buildingClass = GetTileManager()->ObjectFactory->GetAssetClass("Building");
    UClass* buildingClass = GetTileManager()->ObjectFactory->GetAssetClassCategorical("Building");
    if(!buildingClass){
        UE_LOG(LogTemp, Error, TEXT("UrbanTileCurvedRightOuter90: No building class found"));
        return;
    }
    ABuilding* newBuilding = GetWorld()->SpawnActorDeferred<ABuilding>(buildingClass, spawnTransform);
    UGameplayStatics::FinishSpawningActor(newBuilding, spawnTransform);

    ChildActorArray.Add(newBuilding);
    if(newBuilding->SpawnBP){
        ChildActorArray.Add(newBuilding->SubMesh);
    }
}
