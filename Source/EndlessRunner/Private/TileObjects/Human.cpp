// Fill out your copyright notice in the Description page of Project Settings.


#include "Human.h"

#include "EndlessRunnerGameMode.h"

#include "Components/SkeletalMeshComponent.h"
#include "Components/SplineComponent.h"

#include "SegmentationManager.h"
#include "RandomnessManager.h"

// Sets default values
AHuman::AHuman()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AHuman::BeginPlay()
{
	Super::BeginPlay();

    // Setup Materials
    SetupMaterial();

    CalculateNextWalkTargetOnSpline();

    float randomMotionType = (float)GetRandomnessManager()->GetIntInRange(0,5);
    MotionType = randomMotionType;
}

// Called every frame
void AHuman::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    CheckReachedWalkTarget();
    if(IsWalkTargetReached){
        CalculateNextWalkTargetOnSpline();
        Speed = 0.f;
    } else{
        Speed = WalkSpeed;
        Move(DeltaTime);
    }
}

// Called to bind functionality to input
void AHuman::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AHuman::SetupMaterial(){
    // Set dynamic Materials
    for(size_t i=0;i<GetMesh()->GetNumMaterials();i++){
        UMaterialInstanceDynamic* dynamicMatInstance = GetMesh()->CreateAndSetMaterialInstanceDynamicFromMaterial(i, GetMesh()->GetMaterial(i));
        if(dynamicMatInstance && GetComplexityManager()){
            // Adjust dynamic Material according to MaterialActivationPermutation
            GetComplexityManager()->AdjustDynamicMaterial(dynamicMatInstance);
        } else{
            UE_LOG(LogTemp, Error, TEXT("Human: Dynamic Material failed"));
        }
    }
}

AComplexityManager* AHuman::GetComplexityManager(){
    if(GetWorld()){
        if(GetWorld()->GetAuthGameMode()){
            AEndlessRunnerGameMode* GameModeCallback = (AEndlessRunnerGameMode*)GetWorld()->GetAuthGameMode();
            AComplexityManager* complexityManager = GameModeCallback->ComplexityManager;
            return complexityManager;
        }
    }
    UE_LOG(LogTemp, Error, TEXT("Human: GetComplexityManager: No World Found!"));
    return nullptr;
}

ASegmentationManager* AHuman::GetSegmentationManager(){
    if(GetWorld()){
        if(GetWorld()->GetAuthGameMode()){
            AEndlessRunnerGameMode* GameModeCallback = (AEndlessRunnerGameMode*)GetWorld()->GetAuthGameMode();
            ASegmentationManager* segManager = GameModeCallback->SegmentationManager;
            return segManager;
        }
    }
    UE_LOG(LogTemp, Error, TEXT("Human: GetSegmentationManager: No World Found!"));
    return nullptr;
}

ARandomnessManager* AHuman::GetRandomnessManager(){
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

void AHuman::SetSegmentationIndex(FString Name){
    ASegmentationManager* segManager = GetSegmentationManager();
    if(segManager){
        uint8 renderDepth = segManager->GetSegmentationIndex(Name);
        GetMesh()->SetRenderCustomDepth(true);
        GetMesh()->SetCustomDepthStencilValue(renderDepth);
    } else {
        UE_LOG(LogTemp, Error, TEXT("Human: SegmentatonManager is nullptr"));
    }
}

void AHuman::SetGuidingSpline(USplineComponent *GuidingSpline){
    this->GuidingSpline = GuidingSpline;
}

void AHuman::CalculateNextWalkTargetOnSpline(){
    // Check if there is any GuidingSpline available
    if(!GuidingSpline){
        return;
    }
    // Get random point on spline
    float randPercentage = GetRandomnessManager()->GetFloat();
    // Compare against last spline percentage to later determine walking direction
    if(randPercentage >= LastTargetSplinePercentage){
        IsWalkUpLine = true;
    } else{
        IsWalkUpLine = false;
    }
    // Set LastPercentage for next calculation
    LastTargetSplinePercentage = randPercentage;
    // Get MaxSplineLenght for comparison
    float splineLength = GuidingSpline->GetSplineLength();
    // Get Location on Spline at Target
    FVector splineLocation = GuidingSpline->GetLocationAtDistanceAlongSpline(
                splineLength*randPercentage, ESplineCoordinateSpace::World);
    // Set WalkTarget
    WalkTarget = splineLocation;
    // Calculate current distance to WalkTarget as LastDistanceToWalkTarget
    LastDistanceToWalkTarget = FVector::DistXY(GetRootComponent()->GetComponentLocation(), WalkTarget);
}

void AHuman::CheckReachedWalkTarget(){
    // Get current distance to WalkTarget
    float currDist = FVector::DistXY(GetRootComponent()->GetComponentLocation(), WalkTarget);
    // Check against lastDistanceToWalkTarget
    if(currDist > LastDistanceToWalkTarget){
        IsWalkTargetReached = true;
    } else{
        IsWalkTargetReached = false;
    }
    // Store currDist for next tick
    LastDistanceToWalkTarget = currDist;
}

void AHuman::Move(float DeltaTime){
    // Get Vector of rotation at closes spline point
    FVector directionVector = GuidingSpline->FindRotationClosestToWorldLocation(
                GetRootComponent()->GetComponentLocation(), ESplineCoordinateSpace::World).Vector();
    if(!IsWalkUpLine){ // invert rotation vector if need to face other direction
        directionVector *= -1;
    }
    // Rotate Human
    GetRootComponent()->SetWorldRotation(directionVector.Rotation());
    // TODO: add extra rotation so that one is able to move to corner points

    // Move Human
    GetRootComponent()->SetWorldLocation(
                GetRootComponent()->GetComponentLocation() + directionVector*Speed*DeltaTime);

    // Debug
    float distance = GetDistanceAlongSplineClosesToWorldLocaiton(GuidingSpline, GetRootComponent()->GetComponentLocation());
    //UE_LOG(LogTemp, Warning, TEXT("Human: DistanceOnSpline: %f"), distance);
}

float AHuman::GetDistanceAlongSplineClosesToWorldLocaiton(USplineComponent *Spline, FVector WorldLocation){
    float inputKey = Spline->FindInputKeyClosestToWorldLocation(WorldLocation);
    int inputKeyTrauncated = FMath::TruncToInt(inputKey);
    float distancePrevPoint = Spline->GetDistanceAlongSplineAtSplinePoint(inputKey);
    float distanceNextPoint = Spline->GetDistanceAlongSplineAtSplinePoint(inputKey+1);

    float percentage = inputKey - inputKeyTrauncated;
    float distanceDelta = distanceNextPoint - distancePrevPoint;

    float result = percentage * distanceDelta + distancePrevPoint;
    return result;
}

void AHuman::ResetLocation(FVector LocationOffset){
    UE_LOG(LogTemp, Log, TEXT("Human: Resetting human.."));
    // Relocate the Actor according to LocationOffset
    GetRootComponent()->SetWorldLocation(GetRootComponent()->GetComponentLocation() - LocationOffset);
}

void AHuman::ResetWalkTarget(FVector LocationOffset){
    // Relocate the WalkTarget according to LocationOffet
    WalkTarget = WalkTarget - LocationOffset;
}

void AHuman::SetMaxSplineOffset(float MaxSplineOffset){
    this->MaxSplineOffset = MaxSplineOffset;
}
