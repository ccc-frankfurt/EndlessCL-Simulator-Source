// Fill out your copyright notice in the Description page of Project Settings.


#include "MainStreetTile.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SplineComponent.h"

#include "TileManager.h"
#include "RandomnessManager.h"

#include "Tiles/SideWalkTile.h"

#include "TileObjects/Vehicle/AIWheeledVehicle.h"
#include "TileObjects/Vehicle/PlayerWheeledVehicle.h"
#include "TileObjects/ObjectFactory/ObjectFactoryBase.h"

#include "EndlessRunnerGameMode.h"
#include "EndlessRunnerCharacter.h"


// Sets default values
AMainStreetTile::AMainStreetTile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    // Set Class Variables
    MeshSamplePath = TEXT("/Geometry/Street/");
    MaterialSamplePath = TEXT("/Materials/Street/");

    // Load Mesh from Disc (since this is only possible on constructor)
    LoadObjectFromDisc();

    // Load Material from Disc to MaterialArray
    //LoadMaterialFromDisc(MaterialSamplePath);

    // Init BoxCollision
    BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
    BoxCollision->SetupAttachment(Mesh);
    BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &AMainStreetTile::OnBoxOverlap);

    // Init Spawn Arrows
    LeftSpawnArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("LeftSpawnArrow"));
    LeftSpawnArrow->SetupAttachment(Mesh);
    RightSpawnArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("RightSpawnArrow"));
    RightSpawnArrow->SetupAttachment(Mesh);

    // Init debug visual or arrow component
    /*
    ConstructorHelpers::FObjectFinder<UStaticMesh> debugCubeVisual(
            TEXT("StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
    UStaticMeshComponent* debugCube = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DebugCube"));
    debugCube->SetupAttachment(SpawnForwardArrow);
    if(debugCubeVisual.Succeeded()){
        debugCube->SetStaticMesh(debugCubeVisual.Object);
    }
    // Disabling physics response of debug cube
    debugCube->BodyInstance.SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    debugCube->BodyInstance.SetResponseToAllChannels(ECR_Ignore);
    */

   // MATH DEBUG
   //FVector x1 = FVector(0.f, 1.f, 0.f);
   //FVector y1 = FVector(-0.f, -1.f, 0.f);
   //float res1 = GetAnglesBetweenTwoVectors(x1,y1);
   //UE_LOG(LogTemp, Log, TEXT("TEST: %f"), res1);
}

void AMainStreetTile::OnConstruction(const FTransform &transform){
    Super::OnConstruction(transform);

    // Setup MeshSpline
    // Setup Spline
    MeshSpline->SetLocationAtSplinePoint(0, FVector(0.f, 0.f, TileHeight), ESplineCoordinateSpace::Local);
    //MeshSpline->SetTangentsAtSplinePoint(0, FVector(0.f, 0.f, 0.f), FVector(TileWidth*2.4f, 0.f, 0.f), ESplineCoordinateSpace::Local);

    MeshSpline->SetLocationAtSplinePoint(1, FVector(TileLength, 0.f, TileHeight), ESplineCoordinateSpace::Local);
    //MeshSpline->SetTangentsAtSplinePoint(1, FVector(0.f, -TileWidth*2.4f, 0.f), FVector(0.f,0.f,0.f), ESplineCoordinateSpace::Local);

    // Set SpawnForwardArrow to "end" of Tile
    SpawnForwardArrow->SetRelativeLocation(FVector(TileLength, 0.f, 0.f));

    // Setup BoxCollision
    BoxCollision->SetRelativeLocation(FVector(TileLength*0.8, 0.f, 0.f));
    BoxCollision->SetBoxExtent(FVector(50.f, TileWidth*0.8f, 50.f));

    // Setup LeftSpawnArrow and RightSpawnArrow according to Mesh
    LeftSpawnArrow->SetRelativeLocation(FVector(0.f, -TileWidth/2.f, 0.f));
    RightSpawnArrow->SetRelativeLocation(FVector(0.f, TileWidth/2.f, 0.f));
    LeftSpawnArrow->SetRelativeRotation(FRotator(0.f,-90.f,0.f));
    RightSpawnArrow->SetRelativeRotation(FRotator(0.f,90.f,0.f));
    LeftSpawnArrow->SetHiddenInGame(true);
    RightSpawnArrow->SetHiddenInGame(true);

    // Callback TileManager to update "NextSpawn"
    SetNextSpawnAnchor();
    SpawnAdditionalObjects();
}

// Called when the game starts or when spawned
void AMainStreetTile::BeginPlay()
{
	Super::BeginPlay();

    // Set Segmentation Index
    SetSegmentationIndex("Street");

    FRotator spawnRotation = Mesh->GetComponentRotation();

    SpawnSideWalk(FTransform(spawnRotation, LeftSpawnArrow->GetComponentLocation(), FVector::OneVector),
                  LeftSpawnArrow);
    SpawnSideWalk(FTransform(spawnRotation, RightSpawnArrow->GetComponentLocation(), FVector::OneVector),
                  RightSpawnArrow);

}

// Called every frame
void AMainStreetTile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMainStreetTile::Init(){
}

void AMainStreetTile::SetNextSpawnAnchor(){
    // Check if IsMainstreet
    if(!IsMainStreet){
        return;
    }
    // Get TileManager
    if(GetWorld()){
        if(GetWorld()->GetAuthGameMode()){ //needed since else content browser crashes when opening c++ folder
            AEndlessRunnerGameMode* GameModeCallback = (AEndlessRunnerGameMode*)GetWorld()->GetAuthGameMode();
            ATileManager* TileManagerCallback = GameModeCallback->TileManager;
            // Do Callback
            TileManagerCallback->SetMainStreetNextAnchor(SpawnForwardArrow->GetComponentTransform());
        }
    } else{
        UE_LOG(LogTemp, Error, TEXT("AMainStreetTile: No World Found in OnConstruct.."));
    }
}

// FIXME: This may be doable more uniformly(one function -> no copy code?)
void AMainStreetTile::SpawnSideWalk(const FTransform &transform, UArrowComponent *spawnArrow){
    // Spawn new tile
    ASideWalkTile* newTile = GetWorld()->SpawnActorDeferred<ASideWalkTile>(ASideWalkTile::StaticClass(),
                                        transform); // do this with ABaseTile?
    newTile->BaseSpawnTransform = Mesh->GetComponentTransform();
    newTile->ParentForwardVector = SpawnForwardArrow->GetForwardVector();
    newTile->SpawnOffsetDirection = spawnArrow->GetForwardVector();
    UGameplayStatics::FinishSpawningActor(newTile, transform);
    newTile->GetRootComponent()->SetWorldLocation(spawnArrow->GetComponentLocation());
    // Add to this tiles ChildActorArray
    ChildActorArray.Add(newTile);
}


void AMainStreetTile::OnBoxOverlap(UPrimitiveComponent *OverlappedComp, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult){
    //if((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr)){
    //    UE_LOG(LogTemp, Warning, TEXT("BoxCollision Trigger Fired.."));
    //}
    // Check for specific class
    if( (OtherActor->IsA(AEndlessRunnerCharacter::StaticClass())
            || OtherActor->IsA(APlayerWheeledVehicle::StaticClass()))
            && !HasBeenTriggered){
        if(GetWorld()){
            HasBeenTriggered = true;
            AEndlessRunnerGameMode* GameModeCallback = (AEndlessRunnerGameMode*)GetWorld()->GetAuthGameMode();
            ATileManager* TileManagerCallback = GameModeCallback->TileManager;
            // Do Callback
            TileManagerCallback->SpawnMainStreetTile();
            TileManagerCallback->IncreaseTilesPassed();
        } else{
            UE_LOG(LogTemp, Error, TEXT("AMainStreetTile: No World Found in OnConstruct.."));
        }
    }
}

void AMainStreetTile::SpawnAdditionalObjects(){
    ATileManager* tileManager = GetTileManager();
    if(tileManager){
        if(tileManager->CheckIfSpawnActor("Car")){
            SpawnCar(0.5f, true);
        }
    }
}

void AMainStreetTile::SpawnCar(float SplinePercentage, bool IsOncoming){
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
        float laneOffset = GetRandomnessManager()->GetFloatInRange((TileWidth/4.f)-100.f, (TileWidth/4.f)+100.f);
        if(IsOncoming){
            spawnLocation += (rightVector*-1)*laneOffset;
        } else{
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

void AMainStreetTile::ResetChildHandle(FVector LocationOffset){
    //UE_LOG(LogTemp, Log, TEXT("MainStreet: Reset: SplineEnd: %s"),
    //       * MeshSpline->GetLocationAtSplinePoint(1, ESplineCoordinateSpace::World).ToString());
}
