// Fill out your copyright notice in the Description page of Project Settings.


#include "TCrossing.h"

#include "RandomnessManager.h"
#include "TileManager.h"

#include <Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h>

#include "Tiles/MainStreetTile.h"
#include "Tiles/SideWalkTile.h"

#include "Components/SplineComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"

#include "TileObjects/Handler/DummyStreetHandler.h"
#include "TileObjects/Vehicle/PlayerWheeledVehicle.h"

#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"

ATCrossing::ATCrossing(){
    // Init additional Splines
    LeftSpline = CreateDefaultSubobject<USplineComponent>(TEXT("LeftSpline"));
    LeftSpline->SetupAttachment(Mesh);
    RightSpline = CreateDefaultSubobject<USplineComponent>(TEXT("RightSpline"));
    RightSpline->SetupAttachment(Mesh);
    TopSplineRight = CreateDefaultSubobject<USplineComponent>(TEXT("TopSplineRight"));
    TopSplineRight->SetupAttachment(Mesh);
    TopSplineLeft = CreateDefaultSubobject<USplineComponent>(TEXT("TopSplineLeft"));
    TopSplineLeft->SetupAttachment(Mesh);

    // Init ArrowComponents for additional Tiles
    LeftTileSpawnArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("LeftTileSpawnArrow"));
    LeftTileSpawnArrow->SetupAttachment(Mesh);
    RightTileSpawnArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("RightTileSpawnArrow"));
    RightTileSpawnArrow->SetupAttachment(Mesh);

    // Init ArrowComponents for Sidewalk and Terrain Objects
    LeftNextSpawnArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("LeftNextSpawnArrow"));
    LeftNextSpawnArrow->SetupAttachment(Mesh);
    RightNextSpawnArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("RightNextSpawnArrow"));
    RightNextSpawnArrow->SetupAttachment(Mesh);

    TopNextSpawnArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("TopNextSpawnArrow"));
    TopNextSpawnArrow->SetupAttachment(Mesh);
    BottomNextSpawnArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("BottomNextSpawnArrow"));
    BottomNextSpawnArrow->SetupAttachment(Mesh);

    // Init ArrowComponents for DummyStreet spawn
    DummyStreetSpawnArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("DummySpawnArrow"));
    DummyStreetSpawnArrow->SetupAttachment(Mesh);

    // Init BoxCollision
    BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
    BoxCollision->SetupAttachment(Mesh);
    BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &ATCrossing::OnBoxOverlap);

    // Init RoadBlocks
    ConstructorHelpers::FObjectFinder<UStaticMesh> defaultRoadBlockCube(
            TEXT("StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
    RoadBlock0 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RoadBlock0"));
    RoadBlock1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RoadBlock1"));
    RoadBlock2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RoadBlock2"));
    if(defaultRoadBlockCube.Succeeded()){
        RoadBlock0->SetStaticMesh(defaultRoadBlockCube.Object);
        RoadBlock1->SetStaticMesh(defaultRoadBlockCube.Object);
        RoadBlock2->SetStaticMesh(defaultRoadBlockCube.Object);
    }
    RoadBlock0->SetupAttachment(Mesh);
    RoadBlock1->SetupAttachment(Mesh);
    RoadBlock2->SetupAttachment(Mesh);

    RoadBlock0->SetHiddenInGame(true);
    RoadBlock1->SetHiddenInGame(true);
    RoadBlock2->SetHiddenInGame(true);
    // Call init of RoadBlock collision to disable physical collision though make object linetraceable
    SetupRoadBlockCollision(RoadBlock0);
    SetupRoadBlockCollision(RoadBlock1);
    SetupRoadBlockCollision(RoadBlock2);
    // Enable line trace only on one road block
    DisableRoadBlockCollision(RoadBlock0);
    EnableRoadBlockCollision(RoadBlock1);
    EnableRoadBlockCollision(RoadBlock2);

    // Load Material
    UE_LOG(LogTemp, Log, TEXT("TCrossing: LoadingMaterial"));
    MaterialSamplePath = "/Materials/Street/";
    LoadMaterialFromDisc(MaterialSamplePath);

}

void ATCrossing::OnConstruction(const FTransform &transform){
    Super::OnConstruction(transform);

    if(GetWorld()){ // Check that game is actually running
        if(GetWorld()->GetAuthGameMode()){
            SetNextSpawnArrow();
            SetNextSpawnAnchor();
        }
    }
}

void ATCrossing::BeginPlay(){
    Super::BeginPlay();

    // Set Segmentation Index
    SetSegmentationIndex("Street");

    // Spawn DummyStreet
    SpawnDummySteet(DummyStreetSpawnArrow->GetComponentTransform());

    // SpawnSideWalk
    // Left
    SpawnSideWalkTileByClass(SideWalkLeft,
            FTransform(LeftTileSpawnArrow->GetComponentRotation(),
                       LeftTileSpawnArrow->GetComponentLocation(),
                       FVector::OneVector));
    // Right
    SpawnSideWalkTileByClass(SideWalkRight,
            FTransform(RightTileSpawnArrow->GetComponentRotation(),
                       RightTileSpawnArrow->GetComponentLocation(),
                       FVector::OneVector));

    // Top
    SpawnSideWalkTileByClass(SideWalkTop,
            FTransform(TopNextSpawnArrow->GetComponentRotation(),
                       TopNextSpawnArrow->GetComponentLocation(),
                       FVector::OneVector));
}

void ATCrossing::Tick(float DeltaTime){
    TimeSinceLastTrafficSwitch += DeltaTime;
    if(TimeSinceLastTrafficSwitch > TimeToNextTrafficSwitch){
        TimeSinceLastTrafficSwitch -= TimeToNextTrafficSwitch;
        DoTrafficControl();
    }
}

/*
 * Choose with uniform random probability on which side to continue MainStreet
 * and also set correspondingly the arrow for spawning "DummyStreet"
 */
void ATCrossing::SetNextSpawnArrow(){
    if(GetRandomnessManager()->GetFloat() >= 0.5f){ // Left
        SpawnForwardArrow->SetRelativeTransform(LeftNextSpawnArrow->GetRelativeTransform());
        DummyStreetSpawnArrow->SetRelativeTransform(RightNextSpawnArrow->GetRelativeTransform());
    } else { // Right
        SpawnForwardArrow->SetRelativeTransform(RightNextSpawnArrow->GetRelativeTransform());
        DummyStreetSpawnArrow->SetRelativeTransform(LeftNextSpawnArrow->GetRelativeTransform());
    }
}

void ATCrossing::SpawnSideWalkTileByReference(FString AssetReferenceName,
                            const FTransform &Transform){
    // Get Class Reference
    UClass* referenceClassPtr = LoadObject<UClass>(this,
                    *AssetReferenceName);
    // Spawn Actor by Reference
    ABaseTile* newTile = nullptr;
    if(referenceClassPtr){//nullptr check
        newTile = GetWorld()->SpawnActor<ABaseTile>(referenceClassPtr,
                                    Transform);
    }
    // Add newTile to ChildActorArray
    if(newTile){
        ChildActorArray.Add(newTile);
    }
}

void ATCrossing::SpawnSideWalkTileByClass(UClass *AssetClass, const FTransform &Transform){
    ABaseTile* newTile = nullptr;
    if(AssetClass){ // Check for nullptr
        newTile = GetWorld()->SpawnActor<ABaseTile>(AssetClass, Transform);
    }
    // Add new Tile to ChildActorArry
    if(newTile){ // Check for nullptr
        ChildActorArray.Add(newTile);
    }
}

void ATCrossing::SpawnDummySteet(const FTransform& SpawnTransform){
    ADummyStreetHandler* dummyStreetHandler = nullptr;
    dummyStreetHandler = GetWorld()->SpawnActorDeferred<ADummyStreetHandler>(ADummyStreetHandler::StaticClass(),
                                           SpawnTransform);
    // Set number of tiles to spawn
    dummyStreetHandler->NumTilesToSpawn = 2;
    // Set InitalSpawnAnchor in DummyStreetHandler
    dummyStreetHandler->NextTileSpawnAnchor = SpawnTransform;
    UGameplayStatics::FinishSpawningActor(dummyStreetHandler, SpawnTransform);

    // Add DummyStreetHandler to ChildArray
    ChildActorArray.Add(dummyStreetHandler);
}


void ATCrossing::SetNextSpawnAnchor(){
    GetTileManager()->SetMainStreetNextAnchor(SpawnForwardArrow->GetComponentTransform());
}

USplineComponent* ATCrossing::GetMeshSpline(FVector ActorLocation){
    ETileSides closestSide = GetMeshSplineName(ActorLocation);
    return GetMeshSplineByName(closestSide);
}

USplineComponent* ATCrossing::GetPlayerMeshSpline(FVector ActorLocation){
    ETileSides closestSide = GetMeshSplineName(SpawnForwardArrow->GetComponentLocation());
    return GetMeshSplineByName(closestSide);
}

ETileSides ATCrossing::GetMeshSplineName(FVector ActorLocation){
    // Get Distances to Bottom->Random, Left->LeftSpline, and Right->RightSpline
    ETileSides closestSide = ETileSides::NONE;
    float distBottom = (ActorLocation - BottomNextSpawnArrow->GetComponentLocation()).Size(); // Will be closest to player at all times
    float distLeft = (ActorLocation - LeftNextSpawnArrow->GetComponentLocation()).Size();
    float distRight = (ActorLocation - RightNextSpawnArrow->GetComponentLocation()).Size();
    // Calculate shortest distance
    if(distBottom < distLeft && distBottom < distRight){
        closestSide = ETileSides::BOTTOM;
    } else if(distLeft < distBottom && distLeft < distRight){
        if(GetRandomnessManager()->GetFloat() <= 0.5f){
            closestSide = ETileSides::TOP_LEFT;
        } else{
            closestSide = ETileSides::LEFT;
        }
    } else{
        if(GetRandomnessManager()->GetFloat() <= 0.5f){
            closestSide = ETileSides::TOP_RIGHT;
        } else{
            closestSide = ETileSides::RIGHT;
        }
    }
    return closestSide;
}

ETileSides ATCrossing::GetPlayerMeshSplineName(){
    FVector actorLocation = SpawnForwardArrow->GetComponentLocation();
    // Get Distances to Bottom->Random, Left->LeftSpline, and Right->RightSpline
    ETileSides closestSide = ETileSides::NONE;
    float distBottom = (actorLocation - BottomNextSpawnArrow->GetComponentLocation()).Size(); // Will be closest to player at all times
    float distLeft = (actorLocation - LeftNextSpawnArrow->GetComponentLocation()).Size();
    float distRight = (actorLocation - RightNextSpawnArrow->GetComponentLocation()).Size();
    // Calculate shortest distance
    if(distBottom < distLeft && distBottom < distRight){
        closestSide = ETileSides::BOTTOM;
    } else if(distLeft < distBottom && distLeft < distRight){
        closestSide = ETileSides::LEFT;
    } else{
        closestSide = ETileSides::RIGHT;
    }
    return closestSide;
}

USplineComponent* ATCrossing::GetMeshSplineByName(ETileSides TileSideName){
    // Return respective Spline
    if(TileSideName == ETileSides::BOTTOM){ // if bottom, choose left or right
        if(GetRandomnessManager()->GetFloat() <= 0.5f){
            return RightSpline;
        }
        return LeftSpline;
    } else if(TileSideName == ETileSides::RIGHT){
        return RightSpline;
    } else if(TileSideName == ETileSides::LEFT){
        return LeftSpline;
    } else if(TileSideName == ETileSides::TOP_LEFT){
        return TopSplineLeft;
    } else if(TileSideName ==ETileSides::TOP_RIGHT){
        return TopSplineRight;
    } else{
        UE_LOG(LogTemp, Error, TEXT("TCrossing: SplineName did not match!"));
        return nullptr;
    }
}

void ATCrossing::OnBoxOverlap(UPrimitiveComponent *OverlappedComp, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult){
    if( (OtherActor->IsA(AEndlessRunnerCharacter::StaticClass()) ||
         OtherActor->IsA(APlayerWheeledVehicle::StaticClass()))
            && !HasBeenTriggered){
        if(GetWorld()){
            HasBeenTriggered = true;
            AEndlessRunnerGameMode* GameModeCallback = (AEndlessRunnerGameMode*)GetWorld()->GetAuthGameMode();
            ATileManager* TileManagerCallback = GameModeCallback->TileManager;
            // Do Callback
            TileManagerCallback->SpawnMainStreetTile();
            TileManagerCallback->IncreaseTilesPassed();

        } else{
            UE_LOG(LogTemp, Error, TEXT("ATCrossing: No World Found in OnConstruct.."));
        }
    }
}

// Call this function to setup the collision for a given RoadBlock element
// needs to be called befor Enabel/DisableRaodBlockCollision is usefull
void ATCrossing::SetupRoadBlockCollision(UStaticMeshComponent* RoadBlock){
    RoadBlock->SetCollisionProfileName("Custom...");
    RoadBlock->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    RoadBlock->SetCollisionResponseToAllChannels(ECR_Ignore);
    //RoadBlock->SetHiddenInGame(true);
}

// Call this function to make given RoadBlock element detectable by linetrace (no physics)
void ATCrossing::EnableRoadBlockCollision(UStaticMeshComponent *RoadBlock){
    RoadBlock->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
    //RoadBlock->SetHiddenInGame(false);
}

// Call to make non detectable by linetrace
void ATCrossing::DisableRoadBlockCollision(UStaticMeshComponent *RoadBlock){
    RoadBlock->SetCollisionResponseToAllChannels(ECR_Ignore);
    //RoadBlock->SetHiddenInGame(true);
}

void ATCrossing::DoTrafficControl(){
    // Handle TrafficState
    TrafficState += 1;
    if(TrafficState > 2){
        TrafficState = 0;
    }
    // Set collisions accordingly
    if(TrafficState == 0){
        DisableRoadBlockCollision(RoadBlock0);
        EnableRoadBlockCollision(RoadBlock1);
        EnableRoadBlockCollision(RoadBlock2);
    } else if(TrafficState == 1){
        DisableRoadBlockCollision(RoadBlock1);
        EnableRoadBlockCollision(RoadBlock0);
        EnableRoadBlockCollision(RoadBlock2);
    } else if(TrafficState == 2){
        DisableRoadBlockCollision(RoadBlock2);
        EnableRoadBlockCollision(RoadBlock0);
        EnableRoadBlockCollision(RoadBlock1);
    }
}
