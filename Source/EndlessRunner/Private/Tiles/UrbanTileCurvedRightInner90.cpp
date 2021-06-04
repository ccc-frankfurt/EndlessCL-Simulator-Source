// Fill out your copyright notice in the Description page of Project Settings.


#include "UrbanTileCurvedRightInner90.h"

AUrbanTileCurvedRightInner90::AUrbanTileCurvedRightInner90(){
    // Set Class Variables
    MeshSamplePath = "/Geometry/UrbanCurvedRight90/Inner/";

    // Load Mesh from disc
    LoadObjectFromDisc();
}


void AUrbanTileCurvedRightInner90::OnConstruction(const FTransform &transform){

}

void AUrbanTileCurvedRightInner90::BeginPlay(){
    ABaseTile::BeginPlay();
}
