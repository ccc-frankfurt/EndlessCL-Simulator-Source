// Fill out your copyright notice in the Description page of Project Settings.


#include "SegmentationCaptureActor.h"
#include <Runtime/Engine/Classes/Materials/Material.h>
#include <Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h>


ASegmentationCaptureActor::ASegmentationCaptureActor(){

    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    // Set to be Segmentation Capture
    IsSegmentationCapture = true;

    // Set Class Variables
    ImagePrefix = "Mask_";

    // Load PostProcessMaterial
    ConstructorHelpers::FObjectFinder<UMaterial> matAsset(TEXT("Material'/Game/Materials/PostProcess/SegmentationMat.SegmentationMat'"));
    if(matAsset.Succeeded()){
        PostProcessMaterial = matAsset.Object;
    }


}

void ASegmentationCaptureActor::BeginPlay(){
    Super::BeginPlay();

    // Set PostProcessMaterial in CaptureComponent
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

    /*
    CaptureComponent->ShowFlags.SetAmbientOcclusion(false);
    CaptureComponent->ShowFlags.SetAtmosphericFog(false);

    CaptureComponent->ShowFlags.SetDepthOfField(false);
    CaptureComponent->ShowFlags.SetDiffuse(false);
    CaptureComponent->ShowFlags.SetDirectionalLights(false);
    CaptureComponent->ShowFlags.SetDirectLighting(false);
    CaptureComponent->ShowFlags.SetFog(false);
    CaptureComponent->ShowFlags.SetGlobalIllumination(false);


    */
    //CaptureComponent->ShowFlags.SetDeferredLighting(false);
    //CaptureComponent->ShowFlags.SetLighting(false);
    CaptureComponent->ShowFlags.SetDynamicShadows(false);
}

