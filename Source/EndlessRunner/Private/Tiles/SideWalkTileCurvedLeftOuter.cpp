// Fill out your copyright notice in the Description page of Project Settings.


#include "SideWalkTileCurvedLeftOuter.h"

#include "Tiles/UrbanTileCurveLeftOuter.h"

#include "TileObjects/ObjectFactory/ObjectFactoryBase.h"
#include "TileObjects/StreetLampObject.h"
#include "TileObjects/Tree.h"

#include "TileManager.h"
#include "RandomnessManager.h"

#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "Components/SplineComponent.h"

ASideWalkTileCurvedLeftOuter::ASideWalkTileCurvedLeftOuter(){

    // Set Class Variables
    MeshSamplePath = "/Geometry/SideWalkCurvedLeft/Outer/";
    TreeOffsetY.Min = -150;
    TreeOffsetY.Max = -120;

    // Load Mesh from disc
    LoadObjectFromDisc();

}


void ASideWalkTileCurvedLeftOuter::OnConstruction(const FTransform &transform){

    // Setup NextSpawnArrow
    NextSpawnArrow->SetRelativeLocation(FVector(0.f, 330.f, 0.f));
    NextSpawnArrow->SetRelativeRotation(FRotator(0.f, 90.f, 0.f));

    // Setup Spline
    MeshSpline->SetLocationAtSplinePoint(0, FVector(0.f, 330.f/2.f, 0.f), ESplineCoordinateSpace::Local);
    MeshSpline->SetTangentsAtSplinePoint(0, FVector(0.f, 0.f, 0.f), FVector(TileWidth*2.4f, 0.f, 0.f), ESplineCoordinateSpace::Local);

    MeshSpline->SetLocationAtSplinePoint(1, FVector(TileLength*0.95f, -TileWidth*0.9f, 0.f), ESplineCoordinateSpace::Local);
    MeshSpline->SetTangentsAtSplinePoint(1, FVector(0.f, -TileWidth*2.4f, 0.f), FVector(0.f,0.f,0.f), ESplineCoordinateSpace::Local);

    // Setup Additional Objects
    //SpawnAdditionalObjects();
}

void ASideWalkTileCurvedLeftOuter::BeginPlay(){
    ABaseTile::BeginPlay();

    // Setup Additional Objects
    SpawnAdditionalObjects();

    // Spawn People
    SpawnPeople();

    // Set Segmentation Index
    SetSegmentationIndex("SideWalk");

    // Spawn TerrainOuter
    SpawnOuterTerrain(FTransform(
            Mesh->GetComponentRotation(),
            NextSpawnArrow->GetComponentLocation(),
            FVector::OneVector));
}

void ASideWalkTileCurvedLeftOuter::SpawnOuterTerrain(const FTransform& transform){
    // Spawn newTile
    ABaseTile* newTile = nullptr;
    newTile = GetWorld()->SpawnActor<AUrbanTileCurveLeftOuter>(
                AUrbanTileCurveLeftOuter::StaticClass(), transform);

    // Add newTile to ChildActorArray
    ChildActorArray.Add(newTile);
}

void ASideWalkTileCurvedLeftOuter::SpawnAdditionalObjects(){
    ATileManager* tileManager = GetTileManager();
    if(tileManager){
        if(tileManager->CheckIfSpawnActor("Streetlamp")){
            SpawnLampPoles();
        }
        if(tileManager->CheckIfSpawnActor("Tree")){
            SpawnTrees(Mesh->GetComponentTransform());
        }
    }
}

void ASideWalkTileCurvedLeftOuter::SpawnLampPoles(){
    FMinMax numLampsRange = GetTileManager()->GetObjectSamplingRange("Streetlamp");
    int32 numLamps = GetRandomnessManager()->GetIntInRange(numLampsRange.Min, numLampsRange.Max);
    for(size_t i=0;i<numLamps;i++){
        // Get Class of object to spawn
        UClass* streetlampObjClass = GetTileManager()->ObjectFactory->GetAssetClassCategorical("Streetlamp");
        if(!streetlampObjClass){ // nullptr check
            UE_LOG(LogTemp, Error, TEXT("SideWalkTile: StreetlampClass load failed"));
        }

        // Get desired distancce on spline
        float randomSplinePercentage = GetRandomnessManager()->GetFloat();
        // Get Spawn location
        FVector spawnLocation = GetSplineLocationAtDistancePercentage(MeshSpline, randomSplinePercentage);
        // Get offset in Y direction
        int32 randYOffset = -135; // magic numbers for sidewalk tile width
        FVector spawnOffsetVector = GetSplineRightVectorAtDistancePercentage(MeshSpline, randomSplinePercentage);
        spawnOffsetVector *= randYOffset;
        // Get rotation from spline right vector
        FRotator spawnRotation = (spawnOffsetVector * -1).ToOrientationRotator();
        // Assample spawnTransform
        FTransform spawnTransform = FTransform(
                    spawnRotation,
                    spawnLocation+spawnOffsetVector,
                    FVector::OneVector);
        // Spawn Actor
        AStreetLampObject* newLamp = GetWorld()->SpawnActorDeferred<AStreetLampObject>(
                    streetlampObjClass, spawnTransform);
        UGameplayStatics::FinishSpawningActor(newLamp, spawnTransform);
        newLamp->GetRootComponent()->SetWorldTransform(spawnTransform);

        ChildActorArray.Add(newLamp);
    }
}

