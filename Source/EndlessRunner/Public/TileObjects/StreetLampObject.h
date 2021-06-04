// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TileObjects/BaseObject.h"
#include "StreetLampObject.generated.h"

UCLASS()
class ENDLESSRUNNER_API AStreetLampObject : public ABaseObject
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AStreetLampObject();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
    virtual void OnConstruction(const FTransform& transform) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	
};
