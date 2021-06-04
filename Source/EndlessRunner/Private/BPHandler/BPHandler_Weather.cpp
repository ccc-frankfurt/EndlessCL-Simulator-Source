// Fill out your copyright notice in the Description page of Project Settings.


#include "BPHandler_Weather.h"

// Sets default values
ABPHandler_Weather::ABPHandler_Weather()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABPHandler_Weather::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABPHandler_Weather::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

