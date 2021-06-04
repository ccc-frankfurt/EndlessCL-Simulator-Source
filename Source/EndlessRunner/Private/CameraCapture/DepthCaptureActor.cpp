// Fill out your copyright notice in the Description page of Project Settings.


#include "DepthCaptureActor.h"


ADepthCaptureActor::ADepthCaptureActor(){
    PrimaryActorTick.bCanEverTick = true;

    // Set to be DepthCapture
    IsDepthCapture = true;

    // Set Class Variables
    ImagePrefix = "Depth_";

    // Load PostProcessMaterial
    ConstructorHelpers::FObjectFinder<UMaterial> matAsset(TEXT("Material'/Game/Materials/PostProcess/DepthMat.DepthMat'"));
    if(matAsset.Succeeded()){
        PostProcessMaterial = matAsset.Object;
    }
}

void ADepthCaptureActor::BeginPlay(){
    Super::BeginPlay();

    // DepthCapture needs other RenderTarget setup
    //CaptureComponent->CaptureSource = ESceneCaptureSource::SCS_SceneDepth;
    // Set RenderTarget to use FloatOutput
    //RenderTarget->InitCustomFormat(FrameWidth, FrameHeight, PF_FloatRGBA, true);
    // Set RenderTargetTexture to use Float16
    //RenderTarget->RenderTargetFormat = ETextureRenderTargetFormat::RTF_RGBA16f;

    // Set PostProcessingMaterial
    if(PostProcessMaterial){
        CaptureComponent->AddOrUpdateBlendable(PostProcessMaterial);
    }

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

    CaptureComponent->ShowFlags.SetDirectionalLights(false);
    CaptureComponent->ShowFlags.SetDirectLighting(false);
    CaptureComponent->ShowFlags.SetIndirectLightingCache(false);
    CaptureComponent->ShowFlags.SetGlobalIllumination(false);
    CaptureComponent->ShowFlags.SetLightComplexity(false);
    CaptureComponent->ShowFlags.SetLightInfluences(false);
    CaptureComponent->ShowFlags.SetPrecomputedVisibility(false);
    CaptureComponent->ShowFlags.SetShadowFrustums(false);
    CaptureComponent->ShowFlags.SetSkyLighting(false);
    */
    // Deactivating the below magically gives more fps, I dont know why..
    //CaptureComponent->ShowFlags.SetDynamicShadows(false);
    //CaptureComponent->ShowFlags.SetDeferredLighting(false);
    //CaptureComponent->ShowFlags.SetLighting(false);
}
