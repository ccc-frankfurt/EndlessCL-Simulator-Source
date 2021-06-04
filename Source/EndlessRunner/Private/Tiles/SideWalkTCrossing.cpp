// Fill out your copyright notice in the Description page of Project Settings.


#include "SideWalkTCrossing.h"

#include "TileManager.h"

#include "Tiles/BaseTile.h"

#include "Engine/World.h"

#include "Components/StaticMeshComponent.h"

ASideWalkTCrossing::ASideWalkTCrossing(){

    // Set Class Variables
    MaterialSamplePath = TEXT("/Materials/SideWalk/");

    // Load Material from disc
    LoadMaterialFromDisc(MaterialSamplePath);

}


void ASideWalkTCrossing::OnConstruction(const FTransform &transform){

}

void ASideWalkTCrossing::BeginPlay(){
    ABaseTile::BeginPlay();

    // Set Segmentation Index
    SetSegmentationIndex("SideWalk");

    // Spawn Additional Objects
    SpawnAdditionalObjects();

    // Spawn People
    SpawnPeople();

    // SpawnTerrain
    FString LeftTerrainReference = "Class'/Game/Geometry/UrbanTCrossing/Left/UrbanTCrossingCurveLeft_BP.UrbanTCrossingCurveLeft_BP_C'";
    FString RightTerrainReference = "Class'/Game/Geometry/UrbanTCrossing/Right/MyUrbanTCrossingRightTile_BP.MyUrbanTCrossingRightTile_BP_C'";
    FString TopTerrainReference = "Class'/Game/Geometry/UrbanTCrossing/Top/UrbanTCrossingTopTile.UrbanTCrossingTopTile_C'";
    // Left
    if(TileSide == ETileSides::LEFT){
        SpawnTerrainByReference(LeftTerrainReference,
                FTransform(Mesh->GetComponentRotation(),
                           NextSpawnArrow->GetComponentLocation(),
                           FVector::OneVector));

    } else if(TileSide == ETileSides::RIGHT){
        SpawnTerrainByReference(RightTerrainReference,
                FTransform(Mesh->GetComponentRotation(),
                           NextSpawnArrow->GetComponentLocation(),
                           FVector::OneVector));

    } else if(TileSide == ETileSides::TOP){
        SpawnTerrainByReference(TopTerrainReference,
                FTransform(Mesh->GetComponentRotation(),
                           NextSpawnArrow->GetComponentLocation(),
                           FVector::OneVector));
    }

}

void ASideWalkTCrossing::SpawnAdditionalObjects(){
    ATileManager* tileManager = GetTileManager();
    if(tileManager){
        if(tileManager->CheckIfSpawnActor("Streetlamp")){
            SpawnLampPoles(Mesh->GetComponentTransform());
        }
        if(tileManager->CheckIfSpawnActor("Tree")){
            SpawnTrees(Mesh->GetComponentTransform());
        }
    }
}

void ASideWalkTCrossing::SpawnTerrainByReference(FString AssetReference,
                            const FTransform &Transform){

    // Get Class Reference
    UClass* referenceClassPtr = LoadObject<UClass>(this, *AssetReference);
    // Spawn Actor by Reference
    ABaseTile* newTile = nullptr;
    if(referenceClassPtr){
        newTile = GetWorld()->SpawnActor<ABaseTile>(referenceClassPtr,
                                        Transform);
    }
    // Add newTile to ChildActorArray
    if(newTile){
        ChildActorArray.Add(newTile);
    }
}
