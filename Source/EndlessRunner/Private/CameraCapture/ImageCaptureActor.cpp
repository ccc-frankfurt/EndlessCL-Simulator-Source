// Fill out your copyright notice in the Description page of Project Settings.


#include "ImageCaptureActor.h"
#include <Runtime/Engine/Classes/Materials/Material.h>
#include <Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h>


AImageCaptureActor::AImageCaptureActor(){

    PrimaryActorTick.bCanEverTick = true;

    ImagePrefix = "Color_";


    // Load PostProcess Material (StormEffects) // TODO: Remove this hack.. PP material for image may needs to be set differnetly
    ConstructorHelpers::FObjectFinder<UMaterial> matAsset(TEXT("Material'/Game/StormPack/Materials/M_PPScreenEffect.M_PPScreenEffect'"));
    if(matAsset.Succeeded()){
        UE_LOG(LogTemp, Warning, TEXT("Found image pp material"));
        PostProcessMaterial = matAsset.Object;
    }
    else{
        UE_LOG(LogTemp, Warning, TEXT("Found NO image pp material"));
    }

}

void AImageCaptureActor::BeginPlay(){
    Super::BeginPlay();

    // Set PostProcessMaterial
    if(PostProcessMaterial){
        CaptureComponent->AddOrUpdateBlendable(PostProcessMaterial);
        UE_LOG(LogTemp, Warning, TEXT("Set image pp material"));
    }

    // Set SuperSampling for this renderin actor (AA Method)
    CaptureComponent->PostProcessSettings.bOverride_ScreenPercentage = true;
    CaptureComponent->PostProcessSettings.ScreenPercentage = 100.f; // 200.f

    CaptureComponent->ShowFlags.SetTemporalAA(true); // false->FXAA not a good choice..
    CaptureComponent->ShowFlags.SetAntiAliasing(true);
    CaptureComponent->ShowFlags.SetBloom(false);
    CaptureComponent->ShowFlags.SetEyeAdaptation(false);
    CaptureComponent->bAlwaysPersistRenderingState = true;
    CaptureComponent->bCaptureEveryFrame = true;
    CaptureComponent->bCaptureOnMovement = false;

}
