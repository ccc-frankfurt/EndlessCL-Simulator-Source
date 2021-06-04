// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Forward Declaration
class ATileManager;
class ASegmentationManager;
class AComplexityManager;
class ARandomnessManager;
class USplineComponent;

#include "Common/CommonStructs.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/ArrowComponent.h"
#include "BaseTile.generated.h"

UENUM(BlueprintType)
enum class ETileSides : uint8
{
    NONE,
    LEFT,
    RIGHT,
    TOP,
    TOP_LEFT,
    TOP_RIGHT,
    BOTTOM
};

UCLASS()
class ENDLESSRUNNER_API ABaseTile : public AActor
{
	GENERATED_BODY()

protected:
   float TileWidth = 0;
   float TileLength = 0;
   float TileHeight = 0;

   FString SearchEnd = "'";
   FString StaticMeshBasePath = "StaticMesh'/Game";
   FString MaterialBasePath = "Material'/Game";
   FString MaterialInstanceBasePath = "MaterialInstanceConstant'/Game";

public:	
	// Sets default values for this actor's properties
	ABaseTile();

    // Flag to determine if a new environment setting is used on this tile
    bool IsNewEnvironment = false;
    // Struct holding all environment settings structs
    FEnvironmentSettingStruct EnvironmentSettingStruct;

    FString ObjectName = "";

    FMaterialActivationStruct TileMaterialActivationPermutation;
    //int32 TileObjectComplexity;

    USceneComponent* SceneRoot;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BaseTile")
    UStaticMeshComponent* Mesh = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BaseTile")
    USplineComponent* MeshSpline = nullptr;
    UPROPERTY()
    TArray<AActor*> ChildActorArray = TArray<AActor*>(); // For static actors
    UPROPERTY()
    TArray<APawn*> ChildPawnArray = TArray<APawn*>(); // For moving actors

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BaseTile")
    UArrowComponent* SpawnForwardArrow;

    FTransform ParentTransform; //FIXME: not needed?
    FTransform BaseSpawnTransform; // transform of "MainStreetTile" or alternatively the actor "leading" the spawn process
    FVector ParentForwardVector; // direction for "vertical" offset
    FVector SpawnOffsetDirection; // lateral offset
    FString MeshSamplePath = "";
    FString MaterialSamplePath = "";

    TArray<UMaterialInterface*> MaterialArray; // number of entries equals number of material slots


protected:
    // Handle all construction of mesh in this function!
    virtual void OnConstruction(const FTransform& transform) override;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

    void LoadTileMesh(FString assetReference);
    void SetupMaterial();

    UFUNCTION()
    void DestroyChildren(AActor* actor);

    void LoadObjectFromDisc();
    void LoadMaterialFromDisc(FString Path);

    virtual void ResetChildHandle(FVector LocationOffset);

    void SetSegmentationIndex(FString ObjName);

    FString GetRandomAssetPathWithComplexity(FString BasePath, int32 CurrAssetComplexity);
    FMaterialActivationStruct GetMaterialActivationPermutation();
    int32 GetObjectComplexity();

    ATileManager* GetTileManager();
    ARandomnessManager* GetRandomnessManager();
    ASegmentationManager* GetSegmentationManager();
    AComplexityManager* GetComplexityManager();


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

    virtual void Init();

    float GetAnglesBetweenTwoVectors(FVector vec1, FVector vec2);
    FVector GetStaticMeshSize(UStaticMesh* StaticMesh);

    void ResetTileLocation(FVector LocationOffset);

    FVector GetSplineLocationAtDistancePercentage(USplineComponent* Spline, float Percentage);
    FRotator GetSplineRotationAtDistancePercentage(USplineComponent* Spline, float Percentage);
    FVector GetSplineRightVectorAtDistancePercentage(USplineComponent* Spline, float Percentage);

    virtual USplineComponent* GetMeshSpline(FVector ActorLocation);
    virtual USplineComponent* GetPlayerMeshSpline(FVector ActorLocation);
    virtual ETileSides GetMeshSplineName(FVector ActorLocation);
    virtual ETileSides GetPlayerMeshSplineName();
    virtual USplineComponent* GetMeshSplineByName(ETileSides TileSideName);
};
