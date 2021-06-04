// Fill out your copyright notice in the Description page of Project Settings.


#include "MainStreetTile_Curved_Left.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include <Components/StaticMeshComponent.h>

#include "Tiles/SideWalkTileCurvedLeftOuter.h"
#include "Tiles/SideWalkTileCurvedLeftInner.h"

#include "RandomnessManager.h"
#include "TileObjects/ObjectFactory/ObjectFactoryBase.h"
#include "TileObjects/Vehicle/WheeledVehicleBase.h"

#include "Kismet/GameplayStatics.h"


AMainStreetTile_Curved_Left::AMainStreetTile_Curved_Left(){

    MeshSamplePath = "/Geometry/StreetCurvedLeft/";

    // Load Mesh from Disc
    LoadObjectFromDisc();

}

void AMainStreetTile_Curved_Left::OnConstruction(const FTransform &transform){
    // Setup Spawn ArrowComponents
    SpawnForwardArrow->SetRelativeLocation(FVector(TileLength*0.785f, -TileWidth*0.785f, 0.f));
    SpawnForwardArrow->SetRelativeRotation(FRotator(0.f,-90.f,0.f));

    LeftSpawnArrow->SetRelativeLocation(FVector(0.f, -TileWidth*0.21f, 0.f));
    LeftSpawnArrow->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));

    RightSpawnArrow->SetRelativeLocation(FVector(0.f, TileWidth*0.21f, 0.f));
    RightSpawnArrow->SetRelativeRotation(FRotator(0.f, 90.f, 0.f));

    // Setup BoxCollision
    BoxCollision->SetRelativeLocation(FVector(TileLength*0.79f, -TileWidth*0.6f, 10.f));
    BoxCollision->SetBoxExtent(FVector(TileLength*0.21f*2, 50.f, 50.f));

    // Setup Spline
    MeshSpline->SetLocationAtSplinePoint(0, FVector(0.f,0.f,TileHeight), ESplineCoordinateSpace::Local);
    MeshSpline->SetTangentsAtSplinePoint(0, FVector(0.f, 0.f, 0.f), FVector(TileLength*2, 0.f, 0.f), ESplineCoordinateSpace::Local);
    MeshSpline->SetLocationAtSplinePoint(1, FVector(TileLength*0.785f, -TileWidth*0.785f, TileHeight), ESplineCoordinateSpace::Local);
    MeshSpline->SetTangentsAtSplinePoint(1, FVector(0.f, -TileLength*2, 0.f), FVector(0.f, 0.f, 0.f), ESplineCoordinateSpace::Local);
    MeshSpline->SetDrawDebug(true);

    // Set NextSpawnAnchor
    SetNextSpawnAnchor();

    // Spawn Additional Objects
    UE_LOG(LogTemp, Warning, TEXT("Curve: Spawning Additional Objects.."));
    SpawnAdditionalObjects();
}


void AMainStreetTile_Curved_Left::BeginPlay(){
    ABaseTile::BeginPlay();

    // Set Segmentation Index
    SetSegmentationIndex("Street");

    // Spawn outer SideWalkSpawnOuterTerrain
    SpawnSideWalkOuter(FTransform(
            Mesh->GetComponentRotation(),
            RightSpawnArrow->GetComponentLocation(),
            FVector::OneVector));

    // Spawn inner SideWalk
    SpawnSideWalkInner(FTransform(
           Mesh->GetComponentRotation(),
           LeftSpawnArrow->GetComponentLocation(),
           FVector::OneVector));
}

void AMainStreetTile_Curved_Left::SpawnAdditionalObjects(){
    ATileManager* tileManager = GetTileManager();
    if(tileManager){
        if(tileManager->CheckIfSpawnActor("Car")){
            SpawnCar(0.5f, true);
        }
    }
}


void AMainStreetTile_Curved_Left::SpawnCar(float SplinePercentage, bool IsOncoming){
    FMinMax numberOfCars = GetTileManager()->GetObjectSamplingRange("Vehicle");
    int32 numCars = GetRandomnessManager()->GetIntInRange(numberOfCars.Min, numberOfCars.Max);
    //UE_LOG(LogTemp,Warning,TEXT("Spawning Cars: %d"), numCars);
    for(size_t i=0;i<numCars;i++){
        // Get random Spline percentage
        float randomSplinePercentage = GetRandomnessManager()->GetFloatInRange(0.f, 1.f);
        //UE_LOG(LogTemp, Warning, TEXT("MainStreetTile: RandomSplinePercentage: %f"), randomSplinePercentage);
        // Get Transform for spawn
        FVector rightVector = GetSplineRightVectorAtDistancePercentage(MeshSpline, randomSplinePercentage);
        FRotator spawnRotation = GetSplineRotationAtDistancePercentage(MeshSpline, randomSplinePercentage);
        if(IsOncoming){
            spawnRotation = (spawnRotation.Vector()*-1).Rotation();
        }
        FVector spawnLocation = GetSplineLocationAtDistancePercentage(MeshSpline, randomSplinePercentage);
        // Lane Offset
        float laneOffset = GetRandomnessManager()->GetFloatInRange(200.f, 350.f); //magic numbers
        if(IsOncoming){
            //spawnLocation += (rightVector*-1)*TileWidth/4.f;
            spawnLocation += (rightVector*-1)*laneOffset;
        } else{
            //spawnLocation += rightVector*TileWidth/4.f;
            spawnLocation += rightVector*laneOffset;
        }
        // Additional height to prevent "collision on spawn"
        spawnLocation += FVector(0.f, 0.f, 10.f);
        FTransform spawnTransform = FTransform(
                    spawnRotation,
                    spawnLocation,
                    FVector::OneVector);

        // Spawn Actor
        AWheeledVehicleBase* newCar = nullptr;
        UClass* carClass = GetTileManager()->ObjectFactory->GetAssetClassCategorical("Vehicle");
        if(carClass){
            newCar = GetWorld()->SpawnActorDeferred<AWheeledVehicleBase>(carClass, spawnTransform);
            newCar->IsOncoming = IsOncoming;
            UGameplayStatics::FinishSpawningActor(newCar, spawnTransform);
        } else{
            UE_LOG(LogTemp, Error, TEXT("MainStreetTile: Car class was nullptr"));
        }
    }
    
    // Cars do not get added to child arrays as they are not intendet do be destroyed if tile is destroyed
    // Cars do not need to be attached to anything!
}


void AMainStreetTile_Curved_Left::SpawnSideWalkOuter(const FTransform &transform){
    // Spawn Tile
    ASideWalkTileCurvedLeftOuter* newTile = GetWorld()->SpawnActorDeferred<ASideWalkTileCurvedLeftOuter>(
                ASideWalkTileCurvedLeftOuter::StaticClass(), transform);

    UGameplayStatics::FinishSpawningActor(newTile, transform);

    // Add newTile to ChildArray
    ChildActorArray.Add(newTile);

}

void AMainStreetTile_Curved_Left::SpawnSideWalkInner(const FTransform &transform){
    // Spawn Tile
    ASideWalkTileCurvedLeftInner* newTile = GetWorld()->SpawnActor<ASideWalkTileCurvedLeftInner>(
                ASideWalkTileCurvedLeftInner::StaticClass(), transform);

    // Add newTile to ChildArray
    ChildActorArray.Add(newTile);
}
