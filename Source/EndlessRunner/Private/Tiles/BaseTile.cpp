// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseTile.h"
#include "UObject/ConstructorHelpers.h"
#include "HAL/FileManagerGeneric.h"
#include "Misc/Paths.h"
#include "EndlessRunnerGameMode.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SplineComponent.h"
#include "ComplexityManager.h"
#include "RandomnessManager.h"
#include "SegmentationManager.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
ABaseTile::ABaseTile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    // Init Sceen Root
    SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
    RootComponent = SceneRoot;

    // Init Mesh
    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    Mesh->SetupAttachment(SceneRoot);

    // Init Mesh Spline
    MeshSpline = CreateDefaultSubobject<USplineComponent>(TEXT("MeshSpline"));
    MeshSpline->SetMobility(EComponentMobility::Movable);
    MeshSpline->SetupAttachment(Mesh);

    // Init ArrowComponent
    SpawnForwardArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("SpawnForwardArrow"));
    SpawnForwardArrow->SetupAttachment(Mesh);
}

void ABaseTile::OnConstruction(const FTransform &transform){
    Super::OnConstruction(transform);
}


// Called when the game starts or when spawned
void ABaseTile::BeginPlay(){
	Super::BeginPlay();

    // Resize Render Bounds (needed for tesselation)
    float boundScale = 1.6f; // boundScale gives an offset to the object bounds which is needed for tesselation to stop flickering
    Mesh->BoundsScale = boundScale;

    // Store MaterialActivationComplexity of this tile (at time of spawn)
    TileMaterialActivationPermutation = GetMaterialActivationPermutation();

    // Bind OnDestroyed Event
    OnDestroyed.AddDynamic(this, &ABaseTile::DestroyChildren);

    // Call SetupMaterial
    SetupMaterial();

    // Setup SegmentationIndey by Name
    SetSegmentationIndex(ObjectName);
}

// Called every frame
void ABaseTile::Tick(float DeltaTime){
	Super::Tick(DeltaTime);
}

/*
* Aligns Roataion and Offset in world Space of this Actor
* based on parent transfrom.
*/
void ABaseTile::Init(){

}

/*
* Calls back to TileManager to get global complexity entry
*/
FMaterialActivationStruct ABaseTile::GetMaterialActivationPermutation(){
    FMaterialActivationStruct materialActivationPermutation;
    if(GetWorld() && GetComplexityManager()){
        materialActivationPermutation = GetComplexityManager()->MaterialActivationPermutation;
    }
    return materialActivationPermutation;
}

/*
* Calls back to TileManager to get global complexity entry
*/
int32 ABaseTile::GetObjectComplexity(){
    int32 objectComplexity = 0;
    if(GetWorld() && GetComplexityManager()){
        objectComplexity = GetComplexityManager()->ObjectComplexity;
    }
    return objectComplexity;
}

/*
* This function may only be ever called from constructor!
* Loads Mesh (later according to complexity will build its own reference string)
* and sets Tile's width and lenght accordingly
*/
void ABaseTile::LoadTileMesh(FString assetReference){
    ConstructorHelpers::FObjectFinder<UStaticMesh> meshAsset(*assetReference);
    if(meshAsset.Succeeded()){
        Mesh->SetStaticMesh(meshAsset.Object);
        FVector MeshSize = GetStaticMeshSize(meshAsset.Object);
        TileLength = MeshSize.X;//FMath::RoundHalfFromZero(MeshSize.X);
        TileWidth = MeshSize.Y;//FMath::RoundHalfFromZero(MeshSize.Y);
        TileHeight = MeshSize.Z;//FMath::RoundHalfFromZero(MeshSize.Z);
    }
}

void ABaseTile::SetupMaterial(){
    // Check if materials loaded
    if(MaterialArray.Num()>0){
        // Set all Material Slots
        for(size_t i=0;i<Mesh->GetNumMaterials();i++){
            Mesh->SetMaterial(i, MaterialArray[0]);
        }
    }
    // Set dynamic Materials
    for(size_t i=0;i<Mesh->GetNumMaterials();i++){
        UMaterialInstanceDynamic* dynamicMatInstance = Mesh->CreateAndSetMaterialInstanceDynamicFromMaterial(i, Mesh->GetMaterial(i));
        if(dynamicMatInstance && GetComplexityManager()){
            // Adjust dynamic Material according to MaterialActivationComplexity
            GetComplexityManager()->AdjustDynamicMaterial(dynamicMatInstance);
        } else{
            UE_LOG(LogTemp, Error, TEXT("BaseTile: Dynamic Material failed"));
        }
    }
}

FString ABaseTile::GetRandomAssetPathWithComplexity(FString BasePath, int32 CurrAssetComplexity){
    if(BasePath.IsEmpty()){
        UE_LOG(LogTemp, Error, TEXT("BaseTile: GetRandomAsset: Path is empty!"));
        return "";
    }
    TArray<FString> fileNameArray;
    IFileManager &fileManager = IFileManager::Get();
    //FFileManagerGeneric fileManager;
    //fileManager.SetSandboxEnabled(true);

    int32 currComplexity = CurrAssetComplexity;
    FString searchPath = TEXT("");
    FString discPath = "";

    while((fileNameArray.Num()==0) && (currComplexity >= 0)){
        //BasePath += FString::FromInt(currComplexity) + TEXT("/");
        discPath = BasePath + FString::FromInt(currComplexity) + TEXT("/");
        searchPath = FPaths::Combine(*FPaths::ProjectContentDir(), discPath);
        fileManager.FindFiles(fileNameArray, *searchPath);
        //const TCHAR* extension = _T("*.uasset");
        //fileManager.FindFilesRecursive(fileNameArray, *searchPath, extension, true, false, true);
        currComplexity -= 1;
    }
    
    TArray<FString> finalFileNameArray;
    for(FString fname : fileNameArray){
        UE_LOG(LogTemp, Warning, TEXT("FileName: %s"), *fname);
        if(fname.EndsWith(".uasset")){
            finalFileNameArray.Add(fname);
        }
    }

    if((currComplexity == -1) && (fileNameArray.Num() == 0)){
        UE_LOG(LogTemp, Warning, TEXT("BaseTile: No Path for loading materials could be found for any complexity.."));
        return "";
    }
    UE_LOG(LogWindows, Warning, TEXT("BaseTile: NumFilesInArray: %d"), finalFileNameArray.Num());

    for(FString fname : fileNameArray){
        UE_LOG(LogTemp, Warning, TEXT("file: %s"), *fname);
    }
    int32 randomIndex = GetRandomnessManager()->GetIntInRange(0, finalFileNameArray.Num()-1);
    FString outPath = "";
    outPath = finalFileNameArray[randomIndex];
    outPath.RemoveFromEnd(".uasset");
    outPath = discPath + outPath;
    return outPath;
}

/*
* This function may only be called from constructor!
* Loading random object(mesh) from given path
*/
void ABaseTile::LoadObjectFromDisc(){
    if(!GetWorld()){
        UE_LOG(LogWindows, Warning, TEXT("BaseTile: LoadObjectFromDisc: No world found, omit..."));
        return;
    } else{
        UE_LOG(LogWindows, Warning, TEXT("BaseTile: LoadObjectFromDisc: Actually found a world..."));
    }
    FString objPath = GetRandomAssetPathWithComplexity(MeshSamplePath, GetObjectComplexity());
    if(objPath.IsEmpty()){
        UE_LOG(LogTemp, Warning, TEXT("BaseTile: Path is empty:"));
        return;
    }
    ConstructorHelpers::FObjectFinder<UStaticMesh> meshObj(
                *(StaticMeshBasePath + objPath + SearchEnd));
    if(meshObj.Succeeded()){
        // Set Mesh
        Mesh->SetStaticMesh(meshObj.Object);
        // Set Mesh Variables on Tile
        FVector meshSize = GetStaticMeshSize(meshObj.Object);
        TileLength = meshSize.X;//FMath::RoundHalfFromZero(meshSize.X); // X is forward axis
        TileWidth = meshSize.Y;//FMath::RoundHalfFromZero(meshSize.Y); // Y is sidewards
        TileHeight = meshSize.Z;//FMath::RoundHalfFromZero(meshSize.Z); // Z is upwards
    }
}

/*
* This function may only be called from constructor!
* Loading random material from given path
*/
//TODO: Remove dead variable "Path"
void ABaseTile::LoadMaterialFromDisc(FString Path){
    if(GetWorld()){
        if(!GetWorld()->IsGameWorld()){
            return;
        }
    } else{
        return;
    }
    FString matPath = GetRandomAssetPathWithComplexity(MaterialSamplePath, GetObjectComplexity());
    if(matPath.IsEmpty()){
        UE_LOG(LogTemp, Warning, TEXT("BaseTile: Material path is empty!"));
        return;
    }

    ConstructorHelpers::FObjectFinder<UMaterial> materialObj(
                *(MaterialBasePath + matPath + SearchEnd));
    if(materialObj.Succeeded()){
        MaterialArray.Add(materialObj.Object);
    } else {
        ConstructorHelpers::FObjectFinder<UMaterialInterface> materialInstObj(
                    *(MaterialInstanceBasePath + matPath + SearchEnd));
        if(materialInstObj.Succeeded()){
            MaterialArray.Add(materialInstObj.Object);
        }
    }


}

/*
* This method can be used in tiles to handle non static object on reset.
*/
void ABaseTile::ResetChildHandle(FVector LocationOffset){
    // EMPTY
}

void ABaseTile::ResetTileLocation(FVector LocationOffset){
    // Set own tile location according to offset
    FVector currLocation = SceneRoot->GetComponentLocation();
    SceneRoot->SetWorldLocation(currLocation - LocationOffset);
    // Run ResetChildHandl
    ResetChildHandle(LocationOffset);
    // Run thorugh ChildActorArray, if is class BaseTile, call this function recursively
    for(AActor* childActor : ChildActorArray){
        if(childActor->IsA(ABaseTile::StaticClass())){
            ((ABaseTile*)childActor)->ResetTileLocation(LocationOffset);
        }
    }

}

void ABaseTile::DestroyChildren(AActor *actor){
    for(AActor* childActor : ChildActorArray){
        if(childActor){
            if(IsValid(childActor)){
                if(childActor->IsValidLowLevel()){
                    childActor->Destroy();
                } else{
                    UE_LOG(LogTemp, Error, TEXT("BaseTile: ChildActor was not valid on low level.."));
                }
            } else{
                UE_LOG(LogTemp, Error, TEXT("BaseTile: ChildActor was not valid.."));
            }
        } else{
            UE_LOG(LogTemp, Error, TEXT("BaseTile: ChildActor was nullptr!"));
        }
    }
    for(APawn* pawnActor : ChildPawnArray){
        if(pawnActor){
            if(IsValid(pawnActor)){
                if(pawnActor->IsValidLowLevel()){
                    pawnActor->Destroy();
                } else{
                    UE_LOG(LogTemp, Error, TEXT("BaseTile: ChildPawn was not valid on low level!"));
                }
            } else{
                UE_LOG(LogTemp, Error, TEXT("BaseTile: ChildPawn was not valid!"));
            }
        } else{
            UE_LOG(LogTemp, Error, TEXT("BaseTile: ChildPawn was nullptr!"));
        }
    }
}

float ABaseTile::GetAnglesBetweenTwoVectors(FVector vec1, FVector vec2){
    float result = FMath::RoundHalfFromZero(FMath::RadiansToDegrees(acosf(FVector::DotProduct(vec1, vec2))));
    if(result > 181.f){
        //result = FMath::RoundHalfFromZero(FMath::RadiansToDegrees(acosf(FVector::DotProduct(vec1, vec2))));
        UE_LOG(LogTemp, Error, TEXT("BaseTile: GetAnglesBetweenTwoVectors: Maximum Bullshit happend - overflow"));
        result = 180.f; //FIXME: THIS IS A VERY HACKY HACK!
        // this is currently needed because somehting in the above equation is randomly causing overflow givin random rotations to sidewalks ultimately crashing the simulation!
    }
    return result;
}

FVector ABaseTile::GetStaticMeshSize(UStaticMesh* StaticMesh){
    if(!StaticMesh){
        return FVector::ZeroVector;
    }
    return StaticMesh->GetBounds().GetBox().GetSize();
}

ATileManager* ABaseTile::GetTileManager(){
    if(GetWorld()){
        if(GetWorld()->GetAuthGameMode()){
            AEndlessRunnerGameMode* GameModeCallback = (AEndlessRunnerGameMode*)GetWorld()->GetAuthGameMode();
            ATileManager* tileManager = GameModeCallback->TileManager;
            return tileManager;
        }
    }
    return nullptr;
}

ARandomnessManager* ABaseTile::GetRandomnessManager(){
    if(GetWorld()){
        if(GetWorld()->GetAuthGameMode()){
            AEndlessRunnerGameMode* GameModeCallback = (AEndlessRunnerGameMode*)GetWorld()->GetAuthGameMode();
            ARandomnessManager* randomnessManager = GameModeCallback->RandomnessManager;
            return randomnessManager;
        }
    }
    return nullptr;
}

ASegmentationManager* ABaseTile::GetSegmentationManager(){
    if(GetWorld()){
        if(GetWorld()->GetAuthGameMode()){
            AEndlessRunnerGameMode* GameModeCallback = (AEndlessRunnerGameMode*)GetWorld()->GetAuthGameMode();
            ASegmentationManager* segManager = GameModeCallback->SegmentationManager;
            return segManager;
        }
    }
    UE_LOG(LogTemp, Error, TEXT("BaseTile: GetSegmentationManager No World Found!"));
    return nullptr;
}

AComplexityManager* ABaseTile::GetComplexityManager(){
    if(GetWorld()){
        if(GetWorld()->GetAuthGameMode()){
            AEndlessRunnerGameMode* GameModeCallback = (AEndlessRunnerGameMode*)GetWorld()->GetAuthGameMode();
            AComplexityManager* complexityManager = GameModeCallback->ComplexityManager;
            return complexityManager;
        }
    }
    UE_LOG(LogTemp, Error, TEXT("BaseTile: GetComplexityManager: No World Found!"));
    return nullptr;
}

void ABaseTile::SetSegmentationIndex(FString ObjName){
    // Callback Segmentation Manager to get SegmentationIndex by ObjName
    ASegmentationManager* segManager = GetSegmentationManager();
    if(segManager){
        // Activate CustomRenderDepth on Mesh
        Mesh->SetRenderCustomDepth(true);
        // Set CustomRenderDepth
        uint8 renderDepth = segManager->GetSegmentationIndex(ObjName);
        Mesh->SetCustomDepthStencilValue(renderDepth);
    } else{
        UE_LOG(LogTemp, Error, TEXT("BaseTile: SegManager is nullptr"));
    }
}

FVector ABaseTile::GetSplineLocationAtDistancePercentage(USplineComponent *Spline, float Percentage){
    return Spline->GetLocationAtDistanceAlongSpline(Spline->GetSplineLength() * Percentage,
                        ESplineCoordinateSpace::World);
}

FRotator ABaseTile::GetSplineRotationAtDistancePercentage(USplineComponent *Spline, float Percentage){
    return Spline->GetRotationAtDistanceAlongSpline(Spline->GetSplineLength() * Percentage,
                        ESplineCoordinateSpace::World);
}

FVector ABaseTile::GetSplineRightVectorAtDistancePercentage(USplineComponent *Spline, float Percentage){
    return Spline->GetRightVectorAtDistanceAlongSpline(Spline->GetSplineLength() * Percentage,
                        ESplineCoordinateSpace::World);
}

USplineComponent* ABaseTile::GetMeshSpline(FVector ActorLocation){
    return MeshSpline;
}

USplineComponent* ABaseTile::GetPlayerMeshSpline(FVector ActorLocation){
    return MeshSpline;
}

ETileSides ABaseTile::GetMeshSplineName(FVector ActorLocation){
    return ETileSides::NONE;
}

ETileSides ABaseTile::GetPlayerMeshSplineName(){
    return ETileSides::NONE;
}

USplineComponent* ABaseTile::GetMeshSplineByName(ETileSides TileSideName){
    if(TileSideName == ETileSides::NONE){
        return MeshSpline;
    }
    return nullptr;
}
