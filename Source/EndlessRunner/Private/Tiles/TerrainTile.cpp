// Fill out your copyright notice in the Description page of Project Settings.


#include "TerrainTile.h"
#include <Components/StaticMeshComponent.h>
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "TileManager.h"
#include "RandomnessManager.h"

#include "TileObjects/Tree.h"
#include "TileObjects/Building.h"


// Sets default values
ATerrainTile::ATerrainTile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    // Set Class Variables
    MeshSamplePath = TEXT("/Geometry/Terrain/");
    MaterialSamplePath = TEXT("/Materials/Terrain/");

    // Load Mesh
    LoadObjectFromDisc();

    // Load Material
    LoadMaterialFromDisc(MaterialSamplePath);

    // Init NextSpawnArrow
    NextSpawnArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("NextSpawnAnchor"));
    NextSpawnArrow->SetupAttachment(Mesh);
    NextSpawnArrow->SetRelativeLocation(FVector(0.f,0.f,10.f));
    NextSpawnArrow->SetRelativeRotation(FRotator(0.f,90.f,0.f));
    NextSpawnArrow->SetHiddenInGame(true);

}

void ATerrainTile::OnConstruction(const FTransform &transform){
    Super::OnConstruction(transform);

    // Populate Tile
    SpawnAdditionalObjects();

    //  Rotate tile according to SpawnOffsetDirection
    float roatationAngle = GetAnglesBetweenTwoVectors(NextSpawnArrow->GetForwardVector(),
                                                      SpawnOffsetDirection);
    Mesh->SetRelativeRotation(FRotator(0.f,roatationAngle,0.f));
    //Position tile according to SpawnOffsetDirection and ParentForwardVector
    FVector offsetVectorHorizontal = SpawnOffsetDirection * TileWidth/2.f;
    //FVector offsetVectorVertical = ParentForwardVector * TileLength/2.f;
    Mesh->SetWorldLocation(Mesh->GetComponentTransform().GetLocation() + offsetVectorHorizontal);
}

void ATerrainTile::BeginPlay()
{
	Super::BeginPlay();

    // Setup Material
    SetupMaterial();
	
}

// Called every frame
void ATerrainTile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATerrainTile::Init(){
}

void ATerrainTile::SpawnAdditionalObjects(){
    ATileManager* tileManager = GetTileManager();
    if(tileManager){
        if(tileManager->CheckIfSpawnActor("Tree")){
            //SpawnTrees(Mesh->GetComponentTransform());
        }
        if(tileManager->CheckIfSpawnActor("Building")){
            //SpawnBuilding(Mesh->GetComponentTransform());
        }
    }
}

void ATerrainTile::SpawnBuilding(const FTransform &transform){
    // if spawnPercentage is not met: do nothing
    float spawnPercentage = 0.1f;

    if(!GetRandomnessManager()->CheckProbability(spawnPercentage)){
        return;
    }

    ABuilding* newBuilding = GetWorld()->SpawnActorDeferred<ABuilding>(ABuilding::StaticClass(), transform);
    newBuilding->SpawnBoundX = TileLength/2.f;
    newBuilding->SpawnBoundY = TileWidth/2.f;
    UGameplayStatics::FinishSpawningActor(newBuilding, transform);
    newBuilding->AttachToComponent(Mesh, FAttachmentTransformRules::SnapToTargetIncludingScale);
    ChildActorArray.Add(newBuilding);
}

void ATerrainTile::SpawnTrees(const FTransform &transform){
    for(size_t i=0;i<5;i++){
        ATree* newTree = GetWorld()->SpawnActorDeferred<ATree>(ATree::StaticClass(), transform);
        newTree->SpawnBoundX = TileLength/2.f;
        newTree->SpawnBoundY = TileWidth/2.f;
        UGameplayStatics::FinishSpawningActor(newTree, transform);
        newTree->AttachToComponent(Mesh, FAttachmentTransformRules::SnapToTargetIncludingScale);
        ChildActorArray.Add(newTree);
    }
}

