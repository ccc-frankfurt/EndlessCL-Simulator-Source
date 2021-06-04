// Fill out your copyright notice in the Description page of Project Settings.


#include "BP_BaseObject.h"
#include <Components/StaticMeshComponent.h>

// Sets default values
ABP_BaseObject::ABP_BaseObject()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
    RootComponent = SceneRoot;

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    Mesh->SetupAttachment(SceneRoot);

}

// Called when the game starts or when spawned
void ABP_BaseObject::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABP_BaseObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

