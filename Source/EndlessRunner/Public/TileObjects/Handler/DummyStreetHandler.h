// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DummyStreetHandler.generated.h"

UCLASS()
class ENDLESSRUNNER_API ADummyStreetHandler : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADummyStreetHandler();

    int32 NumTilesToSpawn = 1;
    FTransform NextTileSpawnAnchor = FTransform();

    TArray<AActor*> ChildActorArray = TArray<AActor*>();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

    UFUNCTION()
    void DestroyChildren(AActor* Actor);

public:	

    void SpawnDummyStreet();
	
};
