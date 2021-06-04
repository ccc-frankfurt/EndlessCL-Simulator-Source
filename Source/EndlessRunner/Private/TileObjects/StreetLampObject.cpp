// Fill out your copyright notice in the Description page of Project Settings.


#include "StreetLampObject.h"
#include <Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h>
#include <Components/StaticMeshComponent.h>

// Sets default values
AStreetLampObject::AStreetLampObject()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    // Set Class Variables
    //ObjectBasePath = TEXT("/Geometry/SideWalk/Additional/");
    ObjectName = "Streetlamp";

    //LoadMesh("StaticMesh'/Game/Geometry/SideWalk/Additional/0/Lamp/simplePole_10cmx4m.simplePole_10cmx4m'");
    //LoadObjectFromDisc();


}

void AStreetLampObject::OnConstruction(const FTransform &transform){
    Super::OnConstruction(transform);

    // Adjust Relative Location
    // (This needed to be removed for spawn positions to function properly)
    //Mesh->SetRelativeLocation(FVector(SpawnOffsetX, SpawnOffsetY, 0.f));

    // Adjust Relative Roatiaon
    //Mesh->SetRelativeRotation(FRotator(0.f, -RotationZOffset, 0.f));
}

// Called when the game starts or when spawned
void AStreetLampObject::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AStreetLampObject::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

}

