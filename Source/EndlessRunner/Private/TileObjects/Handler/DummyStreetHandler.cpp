// Fill out your copyright notice in the Description page of Project Settings.


#include "DummyStreetHandler.h"

#include "Tiles/MainStreetTile.h"
#include "Tiles/MainStreetTileCurvedRight90.h"
#include "Tiles/MainStreetTile_Curved_Left.h"
#include "Tiles/TCrossing.h"
#include "Tiles/BaseTile.h"

#include "EndlessRunnerGameMode.h"

#include "Engine/World.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"

// Sets default values
ADummyStreetHandler::ADummyStreetHandler()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void ADummyStreetHandler::BeginPlay()
{
	Super::BeginPlay();

    // Bind Destroy Function
    OnDestroyed.AddDynamic(this, &ADummyStreetHandler::DestroyChildren);

    // TODO: Set NextSpawnAnchor
    UE_LOG(LogTemp, Warning, TEXT("Transform: %s"),*NextTileSpawnAnchor.GetLocation().ToString());
    SpawnDummyStreet();
}

void ADummyStreetHandler::SpawnDummyStreet(){
    for(size_t i=0;i<NumTilesToSpawn;i++){
        // Spawn dummy street
        ABaseTile* newTile = nullptr;
        if(i == NumTilesToSpawn-1){ // Last Tile
            newTile = GetWorld()->SpawnActorDeferred<AMainStreetTileCurvedRight90>(AMainStreetTileCurvedRight90::StaticClass(),
                                                    NextTileSpawnAnchor);
            //newTile = GetWorld()->SpawnActorDeferred<AMainStreetTile_Curved_Left>(AMainStreetTile_Curved_Left::StaticClass(),
            //                                        NextTileSpawnAnchor);
        } else{
            newTile = GetWorld()->SpawnActorDeferred<AMainStreetTile>(AMainStreetTile::StaticClass(),
                                        NextTileSpawnAnchor);
        }
        // Set IsMainStreet to false
        AMainStreetTile* mainStreetPtr = Cast<AMainStreetTile>(newTile);
        ATCrossing* tCrossPtr = Cast<ATCrossing>(newTile);
        if(mainStreetPtr){
            mainStreetPtr->IsMainStreet = false;
        }
        UGameplayStatics::FinishSpawningActor(newTile, NextTileSpawnAnchor);
        // Add to child array
        ChildActorArray.Add(newTile);
        // Update NextTileSpawnAnchor
        NextTileSpawnAnchor = newTile->SpawnForwardArrow->GetComponentTransform();
    }
}

void ADummyStreetHandler::DestroyChildren(AActor *Actor){
    for(AActor* childActor : ChildActorArray){
        if(IsValid(childActor)){
            childActor->Destroy();
        }
    }
}
