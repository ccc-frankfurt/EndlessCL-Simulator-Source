// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Forward Declaration
class ATileManager;
class ASegmentationManager;
class AComplexityManager;
class ARandomnessManager;

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TileObjects/BP_BaseObject.h"
#include "BaseObject.generated.h"

UCLASS()
class ENDLESSRUNNER_API ABaseObject : public AActor
{
	GENERATED_BODY()

protected:
    FString BluePrintBasePath = "Class'/Game";
    FString StaticMeshBasePath = "StaticMesh'/Game";
    FString SearchEnd = "'";
    FString BlueprintSearchEnd = "_C'";
	
public:	
	// Sets default values for this actor's properties
	ABaseObject();

    USceneComponent* SceneRoot = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BaseObject")
    UStaticMeshComponent* Mesh = nullptr;

    bool SpawnBP = false;
    TSubclassOf<class ABP_BaseObject> SubClass;
    ABP_BaseObject* SubMesh = nullptr;

    FString ObjectBasePath = "";
    FString ObjectName = "";

    FString MeshSamplePath = "";
    int MeshComplexity = 0;

    int SpawnOffsetX = 0;
    int SpawnOffsetY = 0;

    int32 SpawnBoundX = 0;
    int32 SpawnBoundY = 0;

    float RotationZOffset = 0.f;


protected:
    virtual void OnConstruction(const FTransform& transform) override;
	virtual void BeginPlay() override;

    virtual void LoadMesh(FString assetReference);

    FString GetRandomAssetPathWithComplexity(int32 CurrAssetComplexity);
    void LoadObjectFromDisc();

    void SetupMaterial();

    int32 GetObjectComplexity();

    ASegmentationManager* GetSegmentationManager();
    AComplexityManager* GetComplexityManager();
    ARandomnessManager* GetRandomnessManager();

    void SetSegmentationIndex(FString ObjName);


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

    void SetSpawnOffset(float MinX, float MinY, float MaxX, float MaxY);
	
};
