// Fill out your copyright notice in the Description page of Project Settings.


#include "Tree.h"
#include <Components/StaticMeshComponent.h>
#include <Runtime/Engine/Classes/Materials/MaterialInstanceConstant.h>
#include "RandomnessManager.h"

ATree::ATree(){
    PrimaryActorTick.bCanEverTick = true;

    // Set Class Variables
    ObjectBasePath = "/Geometry/SideWalk/Additional/";
    ObjectName = "Tree";

    // Load Mesh
    //LoadObjectFromDisc();

}

void ATree::OnConstruction(const FTransform &transform){
    Super::OnConstruction(transform);

    // Apply offset to spawn location
    /*
    if(SpawnOffsetX != 0 || SpawnOffsetY != 0){
        Mesh->SetRelativeLocation(FVector(SpawnOffsetX, SpawnOffsetY, 20.f));
    } else{
        if(SpawnBoundX != 0 || SpawnBoundY != 0){
            // Get location
            float randXOffset = 0.f;
            float randYOffset = 0.f;
            if(GetRandomnessManager()){
                randXOffset = GetRandomnessManager()->GetFloatInRange(-SpawnBoundY+Mesh->Bounds.BoxExtent.Y*2,
                                                                            SpawnBoundY-Mesh->Bounds.BoxExtent.Y*2);

                randYOffset = GetRandomnessManager()->GetFloatInRange(-SpawnBoundY+Mesh->Bounds.BoxExtent.Y*2,
                                                                            SpawnBoundY-Mesh->Bounds.BoxExtent.Y*2);
            }

            Mesh->SetRelativeLocation(FVector(randXOffset, randYOffset, 0.f));
            // Check location
            FHitResult hit;
            CheckLocationForCollsion(hit);
            if(hit.IsValidBlockingHit()){
                UE_LOG(LogTemp, Warning, TEXT("Hit"));
            } else{
                //UE_LOG(LogTemp, Warning, TEXT("No Hit"));
            }
        }
    }*/

    // Apply random rotation
    float randomRotation = 0.f;
    if(GetRandomnessManager()){
        randomRotation = GetRandomnessManager()->GetFloatInRange(0.f, 360.f);
    }
    Mesh->SetRelativeRotation(FRotator(0.f, randomRotation, 0.f));
}


void ATree::CheckLocationForCollsion(FHitResult &HitResult){
    // get location above current position
    FVector StartLocation = FVector(Mesh->GetComponentLocation().X, Mesh->GetComponentLocation().Y, 1000.f);
    float groundOffset = 50.f; //
    FVector EndLocation = StartLocation - FVector(0.f,0.f, groundOffset);

    FCollisionQueryParams CollisionParameters;

    ActorLineTraceSingle(HitResult, StartLocation, EndLocation, ECollisionChannel::ECC_WorldDynamic,
                         CollisionParameters);
}
