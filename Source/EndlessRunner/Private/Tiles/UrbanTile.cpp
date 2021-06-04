// Fill out your copyright notice in the Description page of Project Settings.


#include "UrbanTile.h"
#include "Engine/World.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include <Components/StaticMeshComponent.h>
#include "RandomnessManager.h"
#include "TileManager.h"
#include "TileObjects/ObjectFactory/ObjectFactoryBase.h"

#include "TileObjects/UrbanHouse.h"
#include "TileObjects/Building.h"

AUrbanTile::AUrbanTile(){
    PrimaryActorTick.bCanEverTick = true;

    // Set Class Variables
    MeshSamplePath = TEXT("/Geometry/Urban/");
    MaterialSamplePath = TEXT("/Materials/Urban/");

    // Load Mesh
    LoadObjectFromDisc();

    // Load Material
    //LoadMaterialFromDisc(MaterialSamplePath);

    // Init NextSpawnArrow
    NextSpawnArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("NextSpawnActor"));
    NextSpawnArrow->SetupAttachment(Mesh);
    NextSpawnArrow->SetRelativeLocation(FVector(0.f,0.f,10.f));
    NextSpawnArrow->SetRelativeRotation(FRotator(0.f,90.f,0.f));
    NextSpawnArrow->SetHiddenInGame(true);
    //NextSpawnArrow->SetArrowColor(FLinearColor(0.f, 0.f, 255.f)); // Blue

    // Init BuildingSpawn ArrowComponents
    BuildingSpawnA = CreateDefaultSubobject<UArrowComponent>(TEXT("BuildingSpawnA"));
    BuildingSpawnA->SetupAttachment(Mesh);
    BuildingSpawnB = CreateDefaultSubobject<UArrowComponent>(TEXT("BuildingSpawnB"));
    BuildingSpawnB->SetupAttachment(Mesh);

    BuildingSpawnA->SetRelativeLocation(FVector(TileLength/4.f, -TileWidth/4.f, 10.f));
    BuildingSpawnA->SetRelativeRotation(FRotator(0.f,-90.f,0.f));

    BuildingSpawnB->SetRelativeLocation(FVector(-TileLength/4.f, -TileWidth/4.f, 10.f));
    BuildingSpawnB->SetRelativeRotation(FRotator(0.f,-90.f,0.f));

    // TODO: Remove this debug
    BuildingSpawnA->SetHiddenInGame(true);
    BuildingSpawnA->SetArrowColor(FLinearColor(0.f,0.f,255.f)); //Blue
    BuildingSpawnB->SetHiddenInGame(true);
    BuildingSpawnB->SetArrowColor(FLinearColor(0.f,0.f,255.f)); //Blue
}

void AUrbanTile::OnConstruction(const FTransform &transform){
    Super::OnConstruction(transform);

    // Populate Tile
    //SpawnAdditionalObjects();

    // Rotate tile according to SpawnOffsetDirection
    float roatationAngle = GetAnglesBetweenTwoVectors(NextSpawnArrow->GetForwardVector(),
                                                      SpawnOffsetDirection);
    Mesh->SetRelativeRotation(FRotator(0.f,roatationAngle,0.f));

    // Position tile according to SpawnOffsetDirection and ParentForwardVector
    FVector offsetVectorHorizontal = SpawnOffsetDirection * TileWidth/2.f;
    Mesh->SetWorldLocation(Mesh->GetComponentLocation() + offsetVectorHorizontal);


}

void AUrbanTile::BeginPlay(){
    Super::BeginPlay();

    // Set Segmentation Index
    SetSegmentationIndex("Terrain");

    // Setup Material
    SetupMaterial();

    // Populate Tile
    SpawnAdditionalObjects();
}

void AUrbanTile::SpawnAdditionalObjects(){
    ATileManager* tileManager = GetTileManager();
    if(tileManager){
        if(tileManager->CheckIfSpawnActor("Building")){
            SpawnBuilding(BuildingSpawnA);
            SpawnBuilding(BuildingSpawnB);
        }
    }
}


void AUrbanTile::SpawnBuilding(UArrowComponent *Spawn){
    UClass* buildingClass = GetTileManager()->ObjectFactory->GetAssetClassCategorical("Building"); // TODO: "building" should  be a call to an enum to unify the string compare

    if(!buildingClass){ // nullptr check
        UE_LOG(LogTemp, Error, TEXT("UrbanTile: No building class found"));
        return;
    }

    ABuilding* newBuilding = GetWorld()->SpawnActorDeferred<ABuilding>(buildingClass, Spawn->GetComponentTransform());
    newBuilding->SpawnDirection = Spawn->GetForwardVector();
    newBuilding->MinForwardOffset = 450.f;
    newBuilding->MaxForwardOffset = 850.f;
    UGameplayStatics::FinishSpawningActor(newBuilding, Spawn->GetComponentTransform());

    ChildActorArray.Add(newBuilding);

}
