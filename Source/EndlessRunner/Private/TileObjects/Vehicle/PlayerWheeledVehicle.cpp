// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerWheeledVehicle.h"

#include "Common/CommonStructs.h"

#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"

#include "WheeledVehicleMovementComponent.h"

#include "Camera/CameraComponent.h"

#include "Tiles/BaseTile.h"

#include "SegmentationManager.h"
#include "ComplexityManager.h"
#include "SequenceManager.h"
#include "EnvironmentManager.h"

#include "EndlessRunnerGameMode.h"


APlayerWheeledVehicle::APlayerWheeledVehicle(){
    PlayerCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    PlayerCamera->SetupAttachment(GetMesh());

    MaxThrottle = 1.f;

    OnDestroyed.AddDynamic(this, &APlayerWheeledVehicle::WhenDestroyed);

}

void APlayerWheeledVehicle::BeginPlay(){
    Super::BeginPlay();

    // Sync IsSelfDriving flag with segmentation managers IsCapturing flag
    //IsSelfDriving = GetSegmentationManager()->IsCaptureing;
}

void APlayerWheeledVehicle::Tick(float DeltaTime){
    Super::Tick(DeltaTime);

    // Sync IsSelfDriving flag with segmentation managers IsCapturing flag
    IsSelfDriving = GetSegmentationManager()->IsCaptureing;

    // (Re)Set Materials
    // TODO: Check if there has been a change in material permutation every x seconds

    // Check if environment has changed
    if(TileProbedLast){ // nullptr check
        if(TileProbedLast->IsNewEnvironment){
            // Call SequenceManager to update JSON
            AEndlessRunnerGameMode* gameModeCallback = (AEndlessRunnerGameMode*)GetWorld()->GetAuthGameMode();
            ASequenceManager* sequenceManager = gameModeCallback->SequenceManager;
            sequenceManager->AddToSequenceJSON(TileProbedLast->EnvironmentSettingStruct);
            // Call EnvironmentManger to update the environment
            AEnvironmentManager* environmentManager = gameModeCallback->EnvironmentManager;
            environmentManager->SetNewEnvironment(TileProbedLast->EnvironmentSettingStruct.EnvironmentStruct);
            // Reset IsNewEnvironment flag (as it is no longer new)
            TileProbedLast->IsNewEnvironment = false;
        }
    }

    // Do self driving
    if(IsSelfDriving){
        // Do line trace
        if(NumFramesTillTrace % FramesToNextLineTrace == 0){
            DoLineTrace();
        } else{
            NumFramesTillTrace += 1;
        }

        if(GuideSpline){ // if there is a guidespline
            // Check for obstacles
            DoForwardLineTrace();
            // Steering
            FollowGuideSpline();
            GetVehicleMovement()->SetSteeringInput(GetCurrSteering());
            // Throttel
            AdjustThrottle();
            GetVehicleMovement()->SetThrottleInput(CurrThrottel);
        }
    }

}

USplineComponent* APlayerWheeledVehicle::GetNextGuideSpline(ABaseTile *tilePtr, FVector actorLocation){
    return tilePtr->GetPlayerMeshSpline(GetActorLocation()); // TODO: remove argument as it is not needed
}

ETileSides APlayerWheeledVehicle::GetNextGuideSplineName(ABaseTile *tilePtr, FVector actorLocation){
    return tilePtr->GetPlayerMeshSplineName();
}

void APlayerWheeledVehicle::WhenDestroyed(AActor* Actor){
    UE_LOG(LogTemp, Error, TEXT("Simulation is stopped because player got destroyed!"));
    GetWorld()->GetFirstPlayerController()->ConsoleCommand("quit");
}
