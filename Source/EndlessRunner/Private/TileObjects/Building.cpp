// Fill out your copyright notice in the Description page of Project Settings.


#include "Building.h"
#include <Components/StaticMeshComponent.h>
#include "RandomnessManager.h"

#include "Components/ArrowComponent.h"

ABuilding::ABuilding(){
    PrimaryActorTick.bCanEverTick = true;

    DirectionVector = CreateDefaultSubobject<UArrowComponent>(TEXT("DirectionVector"));
    DirectionVector->SetupAttachment(SceneRoot);

    ObjectName = "Building";
}


void ABuilding::OnConstruction(const FTransform &transform){
    Super::OnConstruction(transform);

}

void ABuilding::BeginPlay(){
    Super::BeginPlay();
    // TODO: building needs to adapt rotation of SpawnDirection
    // Calculate rotation difference
    // rotate object by rotation difference

    // Apply random offset to Building
    if(GetRandomnessManager()){
        float randomOffsetY = GetRandomnessManager()->GetFloatInRange(
                    MinForwardOffset, MaxForwardOffset); // magic numbers fit for urban tile
        FVector offsetVector = SpawnDirection * randomOffsetY;
        Mesh->SetWorldLocation(Mesh->GetComponentLocation() + offsetVector);
    }
}
