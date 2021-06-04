// Fill out your copyright notice in the Description page of Project Settings.


#include "UrbanTileCurvedLeftInner.h"

AUrbanTileCurvedLeftInner::AUrbanTileCurvedLeftInner(){

    // Set Class Variables
    MeshSamplePath = "/Geometry/UrbanCurvedLeft/Inner/";

    // Load Mesh from disc
    LoadObjectFromDisc();

}

void AUrbanTileCurvedLeftInner::OnConstruction(const FTransform &transform){

}

void AUrbanTileCurvedLeftInner::BeginPlay(){
    ABaseTile::BeginPlay();
}
