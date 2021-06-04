// Fill out your copyright notice in the Description page of Project Settings.


#include "WheeledVehicleBase.h"

#include "EndlessRunnerGameMode.h"
#include "Engine/World.h"
#include "EngineUtils.h"

#include "Runtime/Engine/Classes/Curves/CurveFloat.h"

#include "SegmentationManager.h"
#include "RandomnessManager.h"
#include "ComplexityManager.h"

#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SplineComponent.h"
#include "Components/ArrowComponent.h"

#include "WheeledVehicleMovementComponent.h"

#include "Tiles/MainStreetTile.h"
#include "Tiles/TCrossing.h"
#include "Common/CommonStructs.h"

#include "DrawDebugHelpers.h"

#include "Classes/Kismet/KismetMathLibrary.h"


AWheeledVehicleBase::AWheeledVehicleBase(){
    PrimaryActorTick.bCanEverTick = true;

    ProbeArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("LeftNextSpawnArrow"));
    ProbeArrow->SetupAttachment(GetMesh());
    //ProbeArrow->SetHiddenInGame(false);

    // DEBUG SPHERE
    DebugSphere = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DebugSphereComponent"));
    static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMeshAsset(TEXT("/Game/StarterContent/Shapes/Shape_Sphere.Shape_Sphere"));
    if (SphereMeshAsset.Succeeded()) {
        DebugSphere->SetStaticMesh(SphereMeshAsset.Object);
        DebugSphere->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
        DebugSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        DebugSphere->SetHiddenInGame(true);
    }
}

void AWheeledVehicleBase::OnConstruction(const FTransform &transform){
    Super::OnConstruction(transform);
}

void AWheeledVehicleBase::BeginPlay(){
    Super::BeginPlay();


    // Setup Segmentation Index
    if(GetSegmentationManager()){ // nullptr check
        SetSegmentationIndex("Car");
    } else{
        UE_LOG(LogTemp, Error, TEXT("WheeledVehicleBase: SegmentationIndex could not be loaded!"));
    }

    // Store current MaterialActivationPermutation
    if(GetComplexityManager()){ // nullptr check
        CurrSelfMaterialActivation = GetComplexityManager()->MaterialActivationPermutation;
    } else {
        // Use default MaterialActivationPermutation
        CurrSelfMaterialActivation = FMaterialActivationStruct();
        UE_LOG(LogTemp, Error, TEXT("WheeledVehicleBase: Initializing with default MaterialActivationPermutation"));
    }
    
    // Setup Materials
    SetupMaterial();

}

void AWheeledVehicleBase::Tick(float DeltaTime){
    Super::Tick(DeltaTime);

    // Check if MaterialActivationPermutation needs to be updated
    if(!(CurrTileMaterialActivation == CurrSelfMaterialActivation)){
        CurrSelfMaterialActivation = CurrTileMaterialActivation;
        // Update Material according to given MaterialActivationPermutation
        SetupMaterial(CurrSelfMaterialActivation);
        UE_LOG(LogTemp, Log, TEXT("Resetting Material on Car based on current Tile"));
    }
}

USplineComponent* AWheeledVehicleBase::GetNextGuideSpline(ABaseTile* tilePtr, FVector actorLocation){
    return tilePtr->GetMeshSpline(actorLocation);
}

ETileSides AWheeledVehicleBase::GetNextGuideSplineName(ABaseTile *tilePtr, FVector actorLocation){
    return tilePtr->GetMeshSplineName(actorLocation);
}

FMaterialActivationStruct AWheeledVehicleBase::GetMaterialActivationFromTile(ABaseTile *tilePtr){
    return tilePtr->TileMaterialActivationPermutation;
}

bool AWheeledVehicleBase::GetCarVisibilityFromTile(ABaseTile *tilePtr){
    return tilePtr->EnvironmentSettingStruct.ObjectApperanceStruct.Car;
}


void AWheeledVehicleBase::DoLineTrace(){
    //FVector traceStart = GetActorLocation() + FVector(0.f, 0.f, 10.f);
    FVector traceStart = ProbeArrow->GetComponentLocation();
    FVector traceForward = FVector(0.f, 0.f, -1.f);
    FVector traceEnd = traceStart + traceForward*100.f;

    //DrawDebugLine(GetWorld(), traceStart, traceEnd, FColor::Green, false, 1, 0, 1);

    FHitResult outHit;
    FCollisionQueryParams CollisionParams;

    // Do LineTrace
    if(GetWorld()->LineTraceSingleByChannel(outHit, traceStart, traceEnd, ECC_Visibility, CollisionParams)){
        if(outHit.bBlockingHit){ //If Hit something
            if(IsValid(outHit.GetActor())){ // Savety check on poniter
                // Check type of hit actor and its name to prevent multiple settings of same actor
                if(outHit.GetActor()->IsA(ABaseTile::StaticClass())
                        && LastGuideSplineName != outHit.GetActor()->GetName()){

                    // Check whether hit tile is valide
                    if(outHit.GetActor()->IsPendingKill()){
                        UE_LOG(LogTemp, Warning, TEXT("Car: Found Actor with pending kill"));
                    }

                    // Renew LastGuideSplineName
                    LastGuideSplineName = outHit.GetActor()->GetName();

                    // Check if tile allows for Car class
                    IsVisibileOnTile = GetCarVisibilityFromTile(Cast<ABaseTile>(outHit.GetActor()));

                    // Check MaterialActivationPermutation of tile
                    CurrTileMaterialActivation = GetMaterialActivationFromTile(Cast<ABaseTile>(outHit.GetActor()));

                    // Check which tile type has been hit
                    USplineComponent* nextGuideSpline = nullptr;
                    AMainStreetTile* mainStreetCastPtr = Cast<AMainStreetTile>(outHit.GetActor());
                    ATCrossing* tCrossPtr = Cast<ATCrossing>(outHit.GetActor());
                    if(mainStreetCastPtr){ // if MainStreetTile
                        nextGuideSpline = GetNextGuideSpline(mainStreetCastPtr, GetActorLocation());
                    } else if(tCrossPtr){ // if TCrossing
                        nextGuideSpline = GetNextGuideSpline(tCrossPtr, GetActorLocation());
                        ETileSides guideSplineName = GetNextGuideSplineName(tCrossPtr, GetActorLocation());
                        nextGuideSpline = tCrossPtr->GetMeshSplineByName(guideSplineName);
                        if(guideSplineName == ETileSides::TOP_LEFT ||
                                guideSplineName == ETileSides::TOP_RIGHT){ // In case of top spline need to switch 'IsOncoming' flag
                            IsOncoming = !IsOncoming;
                        }
                    }
                    // Check if any guidSpline found
                    if(!(nextGuideSpline == nullptr)){
                        UpdateGuideSpline(nextGuideSpline);
                        TileProbedLast = Cast<ABaseTile>(outHit.GetActor());
                    } else{
                        UE_LOG(LogTemp, Error, TEXT("Car: GUIDE SPLINE IS NULLPTR!"));
                    }
                    // Reset NumEmptyLineTraces
                    NumEmptyLineTraces = 0;
                }
            } else{
                UE_LOG(LogTemp, Warning, TEXT("Car: Non-Valid Actor got Hit!"));
            }
        } else{
            UE_LOG(LogTemp, Warning, TEXT("Car: Hit, but was non-blocking"));
        }
    } else{
        NumEmptyLineTraces += 1;
        if(NumEmptyLineTraces % MaxNumEmptyLineTraces == 0){
            this->Destroy(); // Self destruct
        }
    }
}

void AWheeledVehicleBase::DoForwardLineTrace(){
    //FVector traceStart = GetActorLocation() + FVector(0.f, 0.f, 10.f);
    FVector traceStart = ProbeArrow->GetComponentLocation();
    FVector traceForward = ProbeArrow->GetForwardVector();
    FVector traceEnd = traceStart + traceForward * MaxForwardTraceDistance;

    //DrawDebugLine(GetWorld(), traceStart, traceEnd, FColor::Green, false, 1, 0, 1);

    FHitResult outHit;
    FCollisionQueryParams CollisionParams;

    if(GetWorld()->LineTraceSingleByChannel(outHit, traceStart, traceEnd, ECC_Visibility, CollisionParams)){
        CurrDistanceToObstacle = outHit.Distance;
    } else{
        CurrDistanceToObstacle = 10000.f;
    }

}

bool AWheeledVehicleBase::CheckGuideSpline(){
    if(!GuideSpline || !IsValid(GuideSpline)){
        return false;
    }
    if(!GuideSpline->IsValidLowLevel()){
        return false; // TODO: if this occurs we may want to immediately destroy this class too
    }
    return true;
}

void AWheeledVehicleBase::FollowGuideSpline(){
    // Check if GuideSpline still valid
    if(!CheckGuideSpline()){
        CurrSteering = 0.f;
        //UE_LOG(LogTemp, Warning, TEXT("GuideSpline is not valid"));
        return;
    }
    // Get distance of actor along spline
    float actorDistanceOnSpline = FindDistanceAlongSpline(GetActorLocation());
    // Get distance of steering (reference) point along spline
    float steeringPointSplineDistance = actorDistanceOnSpline;
    if(IsOncoming){
        steeringPointSplineDistance -= LookAHeadSteeringDistance;
    } else{
        steeringPointSplineDistance += LookAHeadSteeringDistance;
    }
    // Get world location of steering point
    FVector steeringPointLocation = GuideSpline->GetLocationAtDistanceAlongSpline(steeringPointSplineDistance,
                                                        ESplineCoordinateSpace::World);
    // Offset steering point location with right vector
    FVector splineRightVector = GuideSpline->GetRightVectorAtDistanceAlongSpline(steeringPointSplineDistance,
                                                        ESplineCoordinateSpace::World);
    if(IsOncoming){ // Flip right vector for oncoming vehicles
        splineRightVector *= -1;
    }
    float locationRightOffset = 300.f; // magic number based on street width (TODO: avoid this)
    steeringPointLocation = steeringPointLocation + splineRightVector * locationRightOffset;
    // SET DEBUG SPHERE (TODO: remove)
    DebugSphere->SetWorldLocation(steeringPointLocation);
    /*
    // Get lookAtVector
    FVector lookAtVector = (steeringPointLocation - GetActorLocation());
    // Get angle difference
    float angle = (FMath::Abs(lookAtVector.Rotation().Yaw) - FMath::Abs(GetActorForwardVector().Rotation().Yaw));
    if(lookAtVector.Rotation().Yaw < 0.f){
        angle *= -1;
    }
    */

    // Get angle between two vectors
    FVector lookAtVector = (steeringPointLocation - GetActorLocation());
    lookAtVector.Normalize();
    FVector forwardVector = GetActorForwardVector();
    forwardVector.Normalize();
    // ignore z components
    lookAtVector.Z = 0;
    forwardVector.Z = 0;
    float angle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(lookAtVector, GetActorForwardVector())));
    // Determine sign
    float sign = FVector::CrossProduct(forwardVector, lookAtVector).Z;
    sign = sign * (1.f/FMath::Abs(sign));
    // Apply sign to angle
    angle *= sign;

    // Norm steering
    float steering = angle / MaxSteeringAngle;

    // Set CurrSteering
    CurrSteering = steering;
}

void AWheeledVehicleBase::AdjustThrottle(){
    float throttel = MaxThrottle;
    // Adjust throttel based on steering
    throttel -= (FMath::Abs(GetCurrSteering()) * MaxThrottelDampening);
    // Adjust throttel based on max speed
    float currSpeed = GetVehicleMovement()->GetForwardSpeed() * 0.036; // magic number for km/h conversion
    float maxSpeedPercentage = currSpeed / GetMaxSpeed();
    if(maxSpeedPercentage > 0.95f){
        throttel = (1-maxSpeedPercentage) * throttel; // TODO: this could be a curve as well
    }
    // Adjust throttel based on obstacles
    if(BrakeCurve){ // nullptr check
        if(CurrDistanceToObstacle < MaxForwardTraceDistance){ // override throttel with brake curve
            throttel = BrakeCurve->GetFloatValue((CurrDistanceToObstacle / MaxForwardTraceDistance));
        }
    } else{
        UE_LOG(LogTemp, Error, TEXT("No BrakCurve given.. Car will not brake"));
    }

    // Set throttel
    CurrThrottel = throttel;
}

/*
 * Get the distance along spline closes to the point closes to given WorldLocation
 */
float AWheeledVehicleBase::FindDistanceAlongSpline(FVector WorldLocation){
    // Check if GuideSpline is still valid
    if(!CheckGuideSpline()){
        return -1.f;
    }
    float closesSplineInputKey = GuideSpline->FindInputKeyClosestToWorldLocation(WorldLocation);
    int32 closesSplineInputKeyTruncated = FMath::TruncToInt(closesSplineInputKey);
    float distToKey1 = GuideSpline->GetDistanceAlongSplineAtSplinePoint(closesSplineInputKeyTruncated);
    float distToKey2 = GuideSpline->GetDistanceAlongSplineAtSplinePoint(closesSplineInputKeyTruncated + 1.f);
    float offPercentage = closesSplineInputKey - closesSplineInputKeyTruncated;
    float distanceOnSpline = offPercentage * (distToKey2 - distToKey1);
    if(IsOncoming && distanceOnSpline <= 0.01f){ // Hack to handle cars that are oncomming, else 0 distance is returned which kills cars in curves
        distanceOnSpline = GuideSpline->GetSplineLength();
    }
    return distanceOnSpline;
}


void AWheeledVehicleBase::DestroyOnEmptyLineTraces(){
    // TODO: Move functionality here
}

void AWheeledVehicleBase::UpdateGuideSpline(USplineComponent *Spline){
    GuideSpline = Spline;
}

ASegmentationManager* AWheeledVehicleBase::GetSegmentationManager(){ // Copy from BaseObject.cpp
    if(GetWorld()){
        if(GetWorld()->GetAuthGameMode()){
            AEndlessRunnerGameMode* GameModeCallback = (AEndlessRunnerGameMode*)GetWorld()->GetAuthGameMode();
            ASegmentationManager* segManager = GameModeCallback->SegmentationManager;
            return segManager;
        }
    }
    UE_LOG(LogTemp, Error, TEXT("WheeledVehicleBase: GetSegmentationManager: No World Found!"));
    return nullptr;
}

ARandomnessManager* AWheeledVehicleBase::GetRandomnessManager(){ // Copy from BaseObject.cpp
    if(GetWorld()){
        if(GetWorld()->GetAuthGameMode()){
            AEndlessRunnerGameMode* GameModeCallback = (AEndlessRunnerGameMode*)GetWorld()->GetAuthGameMode();
            ARandomnessManager* randomnessManager = GameModeCallback->RandomnessManager;
            return randomnessManager;
        }
    }
    UE_LOG(LogTemp, Error, TEXT("WheeledVehicleBase: GetRandomnessManager: No World Found!"));
    return nullptr;
}

AComplexityManager* AWheeledVehicleBase::GetComplexityManager(){ // Copy from BaseObject.cpp
    if(GetWorld()){
        if(GetWorld()->GetAuthGameMode()){
            AEndlessRunnerGameMode* GameModeCallback = (AEndlessRunnerGameMode*)GetWorld()->GetAuthGameMode();
            AComplexityManager* complexityManager = GameModeCallback->ComplexityManager;
            return complexityManager;
        }
    }
    UE_LOG(LogTemp, Error, TEXT("WheeledVehicleBase: GetComplexityManager: No World Found!"));
    return nullptr;
}

void AWheeledVehicleBase::SetSegmentationIndex(FString ObjName){
    // Callback Segmentation Manager to get SegmentationIndex by ObjName
    ASegmentationManager* segManager = GetSegmentationManager();
    if(segManager){
        uint8 renderDepth = segManager->GetSegmentationIndex(ObjName);
        // Activate and Set CustomRenderDepth on Mesh
        GetMesh()->SetRenderCustomDepth(true);
        GetMesh()->SetCustomDepthStencilValue(renderDepth);
    } else{
        UE_LOG(LogTemp, Error, TEXT("BaseObject: SegmentationManager is nullptr"));
    }
}

void AWheeledVehicleBase::SetupMaterial(){
    // Set dynamic Materials
    for(size_t i=0;i<GetMesh()->GetNumMaterials();i++){
        UMaterialInstanceDynamic* dynamicMatInstance = GetMesh()->CreateAndSetMaterialInstanceDynamicFromMaterial(i, GetMesh()->GetMaterial(i));
        if(dynamicMatInstance && GetComplexityManager()){
            // Adjust dynamic Material according to currently active MaterialActivationPermutation
            GetComplexityManager()->AdjustDynamicMaterial(dynamicMatInstance);
        } else{
            UE_LOG(LogTemp, Error, TEXT("BaseObj: Dynamic Material failed"));
        }
    }
}

void AWheeledVehicleBase::SetupMaterial(FMaterialActivationStruct MaterialActivationPermutation){
    // Set dynamic Materials
    for(size_t i=0;i<GetMesh()->GetNumMaterials();i++){
        UMaterialInstanceDynamic* dynamicMatInstance = GetMesh()->CreateAndSetMaterialInstanceDynamicFromMaterial(i, GetMesh()->GetMaterial(i));
        if(dynamicMatInstance && GetComplexityManager()){
            // Adjust dynamic Material according to MaterialActivationPermutation
            GetComplexityManager()->AdjustDynamicMaterial(dynamicMatInstance, MaterialActivationPermutation);
        } else{
            UE_LOG(LogTemp, Error, TEXT("BaseObj: Dynamic Material failed"));
        }
    }
}

float AWheeledVehicleBase::GetCurrSteering(){
    return CurrSteering;
}

float AWheeledVehicleBase::GetMaxSpeed(){
    return MaxSpeed;
}
