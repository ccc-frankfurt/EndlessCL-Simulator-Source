// Fill out your copyright notice in the Description page of Project Settings.


#include "SideWalkTile.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SplineComponent.h"
#include "Engine/World.h"

#include "TileManager.h"
#include "RandomnessManager.h"
#include "ComplexityManager.h"
#include "EndlessRunnerGameMode.h"

#include "Tiles/TerrainTile.h"
#include "Tiles/UrbanTile.h"
#include "TileObjects/StreetLampObject.h"
#include "TileObjects/Tree.h"
#include "TileObjects/Human.h"

#include "TileObjects/ObjectFactory/ObjectFactoryBase.h"


// Sets default values
ASideWalkTile::ASideWalkTile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    // Set Class Varaibles
    MeshSamplePath = TEXT("/Geometry/SideWalk/");
    MaterialSamplePath = TEXT("/Materials/SideWalk/");

    TreeOffsetY.Min = 100; // magic number for this tile
    TreeOffsetY.Max = 150; // magic number for this tile

    // Load Mesh from disc
    LoadObjectFromDisc();

    // Load Material from disc
    //LoadMaterialFromDisc(MaterialSamplePath); //FIXME: function argument is doing nothing..

    // Init NextSpawnArrow
    NextSpawnArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("NextSpawnAnchor"));
    NextSpawnArrow->SetupAttachment(Mesh);
    NextSpawnArrow->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
    NextSpawnArrow->SetRelativeLocation(FVector(0.f,-(TileWidth/2.f-30.f), 0.f));
    NextSpawnArrow->SetHiddenInGame(true);

}

void ASideWalkTile::OnConstruction(const FTransform &transform){
    Super::OnConstruction(transform);

    // Setup MeshSpline
    MeshSpline->SetLocationAtSplinePoint(0, FVector(-TileLength/2.f, 0.f, 0.f), ESplineCoordinateSpace::Local);
    MeshSpline->SetTangentsAtSplinePoint(0, FVector(0.f, 0.f, 0.f), FVector(1.f, 0.f, 0.f), ESplineCoordinateSpace::Local);
    MeshSpline->SetLocationAtSplinePoint(1, FVector(TileLength/2.f, 0.f, 0.f), ESplineCoordinateSpace::Local);
    MeshSpline->SetTangentsAtSplinePoint(0, FVector(1.f,0.f,0.f), FVector(0.f,0.f,0.f), ESplineCoordinateSpace::Local);

    // Spawn additional objects
    //SpawnAdditionalObjects();

    // Rotate tile according to SpawnOffsetDirection
    float rotationAngle = GetAnglesBetweenTwoVectors(NextSpawnArrow->GetForwardVector(), SpawnOffsetDirection);
    UE_LOG(LogTemp, Log, TEXT("SideWalk: ForwardVector: %s, SpawnOffsetDirection: %s, RotationAngle: %f"), 
        *NextSpawnArrow->GetForwardVector().ToString(), *SpawnOffsetDirection.ToString(), rotationAngle);
    Mesh->SetRelativeRotation(FRotator(0.f,rotationAngle,0.f));

    // Position tile according to SpawnOffsetDirection and ParentForwardVector
    FVector offsetVectorY = SpawnOffsetDirection * TileWidth/2.f;
    FVector offsetVectorX = ParentForwardVector * TileLength/2.f;
    Mesh->SetWorldLocation(Mesh->GetComponentTransform().GetLocation() + offsetVectorY
                           + offsetVectorX);

}

// Called when the game starts or when spawned
void ASideWalkTile::BeginPlay()
{
	Super::BeginPlay();

    // Spawn additional objects
    SpawnAdditionalObjects();

    // Set Segmentation Index
    SetSegmentationIndex("SideWalk");

    // Spawn People
    SpawnPeople();

    // Spawn Terrain
    FRotator spawnRotation = BaseSpawnTransform.Rotator();
    if(GetWorld()->IsGameWorld()){
        SpawnTerrain(FTransform(spawnRotation, NextSpawnArrow->GetComponentLocation(), FVector::OneVector), NextSpawnArrow);
    }

}

// Called every frame
void ASideWalkTile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASideWalkTile::Init(){
}

void ASideWalkTile::SpawnTerrain(const FTransform &transform, UArrowComponent *spawnArrow){
    // Spawn new tile
    ABaseTile* newTile = nullptr;

    // Get TerrainType from ComplextyManager
    ETerrainTypes terrainType = GetComplexityManager()->GetTerrainType();
    // Check which terrain does apply
    if(terrainType == ETerrainTypes::PARK){
    newTile = GetWorld()->SpawnActorDeferred<ATerrainTile>(ATerrainTile::StaticClass(),
                                        transform);
    } else if(terrainType == ETerrainTypes::URBAN){
    newTile = GetWorld()->SpawnActorDeferred<AUrbanTile>(AUrbanTile::StaticClass(),
                                                                     transform);
    }
    if(newTile){
        newTile->BaseSpawnTransform = BaseSpawnTransform;
        newTile->ParentForwardVector = ParentForwardVector;
        newTile->SpawnOffsetDirection = spawnArrow->GetForwardVector();
        UGameplayStatics::FinishSpawningActor(newTile, transform);
        // Add to this tiles ChildActorArray
        ChildActorArray.Add(newTile);
    }
}


void ASideWalkTile::SpawnAdditionalObjects(){
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

void ASideWalkTile::SpawnLampPoles(const FTransform& transform){
    FMinMax numLampsRange = GetTileManager()->GetObjectSamplingRange("Streetlamp");
    int32 numLamps = GetRandomnessManager()->GetIntInRange(numLampsRange.Min, numLampsRange.Max);
    for(size_t i=0;i<numLamps;i++){
        // Get Class of object to spawn
        UClass* streetlampObjClass = GetTileManager()->ObjectFactory->GetAssetClassCategorical("Streetlamp");
        if(!streetlampObjClass){ // nullptr check
            UE_LOG(LogTemp, Error, TEXT("SideWalkTile: StreetlampClass load failed"));
        }
        
        // Calculate position to spawn object to
        // Get a random percentage (position on the Spline)
        float randomSplinePercentage = GetRandomnessManager()->GetFloat();

        // Get spawn location form Spline percentage
        FVector spawnLocation = GetSplineLocationAtDistancePercentage(MeshSpline, randomSplinePercentage);
        // Get offset in Y direction
        int32 randYOffset = 150; // magic numbers for sidewalk tile width
        // Get the direction to apply the offset
        FVector spawnOffsetVector = GetSplineRightVectorAtDistancePercentage(MeshSpline, randomSplinePercentage);
        spawnOffsetVector *= randYOffset;
        FRotator spawnRotation = (spawnOffsetVector * -1).ToOrientationRotator();
        FTransform spawnTransform = FTransform(
            spawnRotation,
            spawnLocation+spawnOffsetVector,
            FVector::OneVector);
        // Spawn object
        AStreetLampObject* newLamp = GetWorld()->SpawnActorDeferred<AStreetLampObject>(streetlampObjClass, transform);
        UGameplayStatics::FinishSpawningActor(newLamp, transform);
        // Move object according to spline location
        newLamp->GetRootComponent()->SetWorldTransform(spawnTransform);
        //newLamp->GetRootComponent()->SetWorldLocation(spawnLocation+spawnOffsetVector);
        //newLamp->AttachToComponent(Mesh, FAttachmentTransformRules::SnapToTargetIncludingScale);
        ChildActorArray.Add(newLamp);
    }   
}

void ASideWalkTile::SpawnTrees(const FTransform &transform, int32 NumMaxTrees){
    int32 numTrees = 0;
    // Get number of trees to spawn
    FMinMax numTreesRange = GetTileManager()->GetObjectSamplingRange("Tree");
    //numTrees = GetRandomnessManager()->GetIntInRange(1,NumMaxTrees);
    numTrees = GetRandomnessManager()->GetIntInRange(numTreesRange.Min, numTreesRange.Max);
    // Spawn the trees
    for(size_t i=0;i<numTrees;i++){
        float randomSplinePercentage = GetRandomnessManager()->GetFloat();
        // Get random offset on x axis
        //int32 randXOffset = GetRandomnessManager()->GetFloatInRange(-TileLength/2.f, TileLength/2.f);
        FVector spawnLocation = GetSplineLocationAtDistancePercentage(MeshSpline, randomSplinePercentage);
        // Get random offset on y axis
        int32 randYOffset = GetRandomnessManager()->GetIntInRange(TreeOffsetY.Min, TreeOffsetY.Max);
        UE_LOG(LogTemp, Error, TEXT("SideWalkTile: RandomOffsetY: %d, Using: %d , %d"), randYOffset, TreeOffsetY.Min, TreeOffsetY.Max);
        int32 randZOffset = 20; //magic tree offset for sidewalk tile height
        FVector spawnOffsetVector = GetSplineRightVectorAtDistancePercentage(MeshSpline, randomSplinePercentage);
        spawnOffsetVector *= randYOffset;
        spawnOffsetVector += FVector(0.f,0.f,randZOffset);
        // Get Class
        UClass* treeObjClass = GetTileManager()->ObjectFactory->GetAssetClassCategorical("Tree");
        if(!treeObjClass){
            UE_LOG(LogTemp, Error, TEXT("SideWalkTile: TreeClass load failed"));
            return;
        }
        // Spawn actor
        ATree* newTree = GetWorld()->SpawnActorDeferred<ATree>(treeObjClass, transform);
        UGameplayStatics::FinishSpawningActor(newTree, transform);
        // Attach child component to ChildActorArray
        //newTree->AttachToComponent(Mesh, FAttachmentTransformRules::SnapToTargetIncludingScale);
        newTree->GetRootComponent()->SetWorldLocation(spawnLocation+spawnOffsetVector);
        ChildActorArray.Add(newTree);
        // Attach blueprint to ChildActorArray if any spawned
        if(newTree->SpawnBP){
            ChildActorArray.Add(newTree->SubMesh);
        }
    }
}

// TODO: -dynamic loading of people to spawn
void ASideWalkTile::SpawnPeople(){
    // Check whethere to spawn people
    if(!GetTileManager()->CheckIfSpawnActor("People")){
        return;
    }
    FMinMax numberOfPeopleRange = GetTileManager()->GetObjectSamplingRange("Human");
    int32 numPeople = GetRandomnessManager()->GetIntInRange(numberOfPeopleRange.Min, numberOfPeopleRange.Max);
    for(size_t i=0;i<numPeople;i++){
        // Get reference to class
        //UClass* charakterClass = GetTileManager()->ObjectFactory->GetAssetClass("Human");
        UClass* charakterClass = GetTileManager()->ObjectFactory->GetAssetClassCategorical("Human");

        if(charakterClass){
            float randomSplinePercentag = GetRandomnessManager()->GetFloatInRange(0.f, 0.9f);
            // Get SpawnLocation
            FVector spawnLocation = GetSplineLocationAtDistancePercentage(MeshSpline, randomSplinePercentag);
            // Add offset to SpawnLocation
            FVector spawnRightVector = GetSplineRightVectorAtDistancePercentage(MeshSpline, randomSplinePercentag);
            float rightOffset = GetRandomnessManager()->GetFloatInRange(-80.f, 80.f); // magic number = tile width
            /*
            if(GetRandomnessManager()->GetFloat()>=0.5f){
                rightOffset *= -1;
            } else{
                rightOffset -= 50.f; // MagicNumber to prevent walking through trees etc.
            }
            */
            spawnLocation += spawnRightVector*rightOffset;
            FVector randomDirectionVector = FVector(
                        GetRandomnessManager()->GetFloatInRange(-1.f, 1.f),
                        GetRandomnessManager()->GetFloatInRange(-1.f, 1.f),
                        0.f);
            FRotator spawnRotation = randomDirectionVector.Rotation();
            spawnLocation += FVector(0.f, 0.f, 110.f); // zOffset to not fall through floor
            FTransform spawnTransform = FTransform(
                        spawnRotation,
                        spawnLocation,
                        FVector::OneVector);

            // Spawn Class
            AHuman* bot = GetWorld()->SpawnActorDeferred<AHuman>(charakterClass, spawnTransform);
            bot->GuidingSpline = MeshSpline;
            bot->LastTargetSplinePercentage = randomSplinePercentag;
            bot->SetMaxSplineOffset(TileWidth / 2.f);
            UGameplayStatics::FinishSpawningActor(bot, spawnTransform);
            if(bot){
                //bot->AttachToComponent(Mesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
                bot->GetRootComponent()->SetWorldTransform(spawnTransform);
                ChildPawnArray.Add(bot);

                // Set SegmentationIndex for Bot
                bot->SetSegmentationIndex("People");
            } else{
                UE_LOG(LogTemp, Error, TEXT("SideWalk: Still some problem.."));
            }
        } else{
            UE_LOG(LogTemp, Error, TEXT("SideWalk: NULLPTR!"));
        }

    }
}

void ASideWalkTile::ResetChildHandle(FVector LocationOffset){
    for(APawn* pawnActor : ChildPawnArray){
        if(IsValid(pawnActor) && !(pawnActor == nullptr)){
            Cast<AHuman>(pawnActor)->ResetLocation(LocationOffset);
            Cast<AHuman>(pawnActor)->ResetWalkTarget(LocationOffset);
        }
    }
}



