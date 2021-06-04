// Fill out your copyright notice in the Description page of Project Settings.


#include "NormalsCaptureActor.h"


ANormalsCaptureActor::ANormalsCaptureActor(){
    PrimaryActorTick.bCanEverTick = true;

    // Set Class Variables
    ImagePrefix = "Normals_";

    // Load PostProcessMaterial
    ConstructorHelpers::FObjectFinder<UMaterial> matAsset(TEXT("Material'/Game/Materials/PostProcess/NormalMat.NormalMat'"));
    if(matAsset.Succeeded()){
        PostProcessMaterial = matAsset.Object;
    }
}

void ANormalsCaptureActor::BeginPlay(){
    Super::BeginPlay();

    // Set PostProcessingMaterial in CaptureComponent
    if(PostProcessMaterial){
        CaptureComponent->AddOrUpdateBlendable(PostProcessMaterial);
    }

    CaptureComponent->PostProcessSettings.bOverride_ScreenPercentage = true;
    CaptureComponent->PostProcessSettings.ScreenPercentage = 100.f;

    // Deactivate Anti-Aliasing (we dont want interpolatet pixels in segmentation)
    CaptureComponent->ShowFlags.SetTemporalAA(false);
    CaptureComponent->ShowFlags.SetAntiAliasing(false);
    CaptureComponent->ShowFlags.SetBloom(false);
    CaptureComponent->ShowFlags.SetEyeAdaptation(false);
    CaptureComponent->bAlwaysPersistRenderingState = true;
    CaptureComponent->bCaptureEveryFrame = true;
    CaptureComponent->bCaptureOnMovement = false;
}
