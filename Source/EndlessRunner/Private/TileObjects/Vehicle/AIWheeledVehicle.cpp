// Fill out your copyright notice in the Description page of Project Settings.


#include "AIWheeledVehicle.h"

#include <Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h>
#include "Runtime/Engine/Classes/Curves/CurveFloat.h"

#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"

#include "WheeledVehicleMovementComponent.h"


AAIWheeledVehicle::AAIWheeledVehicle(){

}

void AAIWheeledVehicle::OnConstruction(const FTransform &transform){
    Super::OnConstruction(transform);
}

void AAIWheeledVehicle::BeginPlay(){
    Super::BeginPlay();
}


void AAIWheeledVehicle::Tick(float DeltaTime){
    Super::Tick(DeltaTime);

    // Check if on this tile car are allowed
    if(! IsVisibileOnTile){
        this->SetActorHiddenInGame(true);
    } else {
        this->SetActorHiddenInGame(false);
    }

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

