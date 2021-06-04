// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseObject.h"
#include "UObject/ConstructorHelpers.h"
#include "HAL/FileManagerGeneric.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SplineComponent.h"

#include "SegmentationManager.h"
#include "ComplexityManager.h"
#include "RandomnessManager.h"
#include "EndlessRunnerGameMode.h"

#include <Runtime/Engine/Classes/Engine/Blueprint.h>
#include "Engine/World.h"

// Sets default values
ABaseObject::ABaseObject()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    // Init SceeneComponent aka SceneRoot
    SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
    RootComponent = SceneRoot;
    // Init Mesh
    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    Mesh->SetupAttachment(SceneRoot);

}

void ABaseObject::OnConstruction(const FTransform &transform){
    Super::OnConstruction(transform);
}

// Called when the game starts or when spawned
void ABaseObject::BeginPlay()
{
	Super::BeginPlay();

    // Spawn Blueprint Asset
    if(SpawnBP){
        SubMesh = GetWorld()->SpawnActor<ABP_BaseObject>(SubClass, FTransform());
        SubMesh->RegisterAllComponents();
        SubMesh->SetActorLocation(Mesh->GetComponentLocation());
        SubMesh->SetActorRotation(Mesh->GetComponentRotation());
        SubMesh->AttachToComponent(Mesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
        if(!SubMesh){
            UE_LOG(LogTemp, Error, TEXT("BaseObject: SubMesh is null"));
        } else if(!SubMesh->Mesh){
            UE_LOG(LogTemp, Error, TEXT("BaseObject: SubMesh Mesh is null"));
        }
    }

    // Setup Materials
    SetupMaterial();

    // Set Object Segmentation
    SetSegmentationIndex(ObjectName);
}

// Called every frame
void ABaseObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseObject::LoadMesh(FString assetReference){
    ConstructorHelpers::FObjectFinder<UStaticMesh> meshAsset(*assetReference);
    if(meshAsset.Succeeded()){
        Mesh->SetStaticMesh(meshAsset.Object);
    }
}

void ABaseObject::SetSpawnOffset(float MinX, float MinY, float MaxX, float MaxY){
    if(GetRandomnessManager()){
        SpawnOffsetX = GetRandomnessManager()->GetFloatInRange(MinX, MaxX);
        SpawnOffsetY = GetRandomnessManager()->GetFloatInRange(MinY, MaxY);
    }
}

int32 ABaseObject::GetObjectComplexity(){
    int32 objectComplexity = 0;
    if(GetWorld() && GetWorld()->IsGameWorld()){
        objectComplexity = GetComplexityManager()->ObjectComplexity;
    }
    return objectComplexity;
}

FString ABaseObject::GetRandomAssetPathWithComplexity(int32 CurrAssetComplexity){
    TArray<FString> fileNameArray;
    FFileManagerGeneric fileManager;
    fileManager.SetSandboxEnabled(true);

    int32 currComplexity = CurrAssetComplexity;
    FString discPath = "";
    FString searchPath = "";

    while((fileNameArray.Num() == 0) && (currComplexity >= 0)){
        discPath = ObjectBasePath + FString::FromInt(currComplexity) + "/" + ObjectName + "/";
        searchPath = FPaths::Combine(*FPaths::ProjectContentDir(), discPath);
        fileManager.FindFiles(fileNameArray, *searchPath);
        currComplexity -= 1;
    }
    if((currComplexity == -1) && (fileNameArray.Num() == 0)){
        UE_LOG(LogTemp, Warning, TEXT("BaseObject: No Path for loading materials could be found for any complexity.."));
        return "";
    }
    int32 randomIndex = GetRandomnessManager()->GetIntInRange(0, fileNameArray.Num()-1);
    FString assetName = fileNameArray[randomIndex];
    assetName.RemoveFromEnd(".uasset");

    return discPath + assetName;

}

void ABaseObject::LoadObjectFromDisc(){
    if(!(GetWorld() && GetWorld()->IsGameWorld())){
        return;
    }
    // Get object path
    FString objPath = GetRandomAssetPathWithComplexity(GetObjectComplexity());
    // Get back object name from path
    TArray<FString> splitArray;
    objPath.ParseIntoArray(splitArray,TEXT("/"),false);
    FString objName = splitArray.Last();

    if(objPath.IsEmpty()){
        UE_LOG(LogTemp, Warning, TEXT("BaseObj: RandomPath is empty"));
        return;
    }

    if(objPath.Contains("BP")){ // TODO: change this loading
        ConstructorHelpers::FObjectFinder<UBlueprint> bpObj(*
                  (BluePrintBasePath + objPath + SearchEnd));
        if(bpObj.Succeeded()){
            // Load BluePrint
            SubClass = (UClass*)bpObj.Object->GeneratedClass;
            // Set spawn blueprint flag
            SpawnBP = true;
        }
    } else{
        // Load Static Mesh
        ConstructorHelpers::FObjectFinder<UStaticMesh> meshObj(*
                  (StaticMeshBasePath + objPath + SearchEnd));
        if(meshObj.Succeeded()){
            Mesh->SetStaticMesh(meshObj.Object);
        }
    }
}

void ABaseObject::SetupMaterial(){
    // Set dynamic Materials
    for(size_t i=0;i<Mesh->GetNumMaterials();i++){
        UMaterialInstanceDynamic* dynamicMatInstance = Mesh->CreateAndSetMaterialInstanceDynamicFromMaterial(i, Mesh->GetMaterial(i));
        if(dynamicMatInstance && GetComplexityManager()){
            // Adjust dynamic Material according to MaterialActivationComplexity
            GetComplexityManager()->AdjustDynamicMaterial(dynamicMatInstance);
        } else{
            UE_LOG(LogTemp, Error, TEXT("BaseObj: Dynamic Material failed"));
        }
    }
}

ASegmentationManager* ABaseObject::GetSegmentationManager(){
    if(GetWorld()){
        if(GetWorld()->GetAuthGameMode()){
            AEndlessRunnerGameMode* GameModeCallback = (AEndlessRunnerGameMode*)GetWorld()->GetAuthGameMode();
            ASegmentationManager* segManager = GameModeCallback->SegmentationManager;
            return segManager;
        }
    }
    UE_LOG(LogTemp, Error, TEXT("BaseObject: GetSegmentationManager: No World Found!"));
    return nullptr;
}

AComplexityManager* ABaseObject::GetComplexityManager(){
    if(GetWorld()){
        if(GetWorld()->GetAuthGameMode()){
            AEndlessRunnerGameMode* GameModeCallback = (AEndlessRunnerGameMode*)GetWorld()->GetAuthGameMode();
            AComplexityManager* complexityManager = GameModeCallback->ComplexityManager;
            return complexityManager;
        }
    }
    UE_LOG(LogTemp, Error, TEXT("BaseObject: GetComplexityManager: No World Found!"));
    return nullptr;
}

ARandomnessManager* ABaseObject::GetRandomnessManager(){
    if(GetWorld()){
        if(GetWorld()->GetAuthGameMode()){
            AEndlessRunnerGameMode* GameModeCallback = (AEndlessRunnerGameMode*)GetWorld()->GetAuthGameMode();
            ARandomnessManager* randomnessManager = GameModeCallback->RandomnessManager;
            return randomnessManager;
        }
    }
    UE_LOG(LogTemp, Error, TEXT("BaseObject: GetRandomnessManager: No World Found!"));
    return nullptr;
}

void ABaseObject::SetSegmentationIndex(FString ObjName){
    // Callback Segmentation Manager to get SegmentationIndex by ObjName
    ASegmentationManager* segManager = GetSegmentationManager();
    if(segManager){
        uint8 renderDepth = segManager->GetSegmentationIndex(ObjName);
        // Activate and Set CustomRenderDepth on Mesh
        if(SpawnBP){
            // TODO: Loop through all attached StaticMesh components and set their customstencil too
            SubMesh->Mesh->SetRenderCustomDepth(true);
            SubMesh->Mesh->SetCustomDepthStencilValue(renderDepth);
        } else{
            Mesh->SetRenderCustomDepth(true);
            Mesh->SetCustomDepthStencilValue(renderDepth);
        }
    } else{
        UE_LOG(LogTemp, Error, TEXT("BaseObject: SegmentationManager is nullptr"));
    }
}















