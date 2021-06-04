// Fill out your copyright notice in the Description page of Project Settings.


#include "UrbanHouse.h"
#include "Components/ArrowComponent.h"
#include "Components/StaticMeshComponent.h"
#include "RandomnessManager.h"


AUrbanHouse::AUrbanHouse(){

    // Set Class Variables
    ObjectBasePath = TEXT("/Geometry/Urban/Additional/");
    ObjectName = "Building";

    // Load Mesh
    LoadObjectFromDisc();

}


void AUrbanHouse::OnConstruction(const FTransform &Transform){
    Super::OnConstruction(Transform);

    // Position Mesh according to SpawnOffsets
    //UE_LOG(LogTemp, Warning, TEXT("UrbanHouse: ForwardBoundX: %f"), ForwardBoundX);

    // Apply random offset to House
    if(GetRandomnessManager()){
        //TODO: ForwardBound should be MaxForwardBound and ForwardBound with respect to building needs to be calculated
        float randomOffsetY = GetRandomnessManager()->GetFloatInRange(0.f, -ForwardBoundX);
        FVector currLocation = Mesh->GetRelativeTransform().GetLocation();
        currLocation += FVector(0.f,randomOffsetY,0.f);
        Mesh->SetRelativeLocation(currLocation);
    }

}
