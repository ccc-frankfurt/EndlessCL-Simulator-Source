// Fill out your copyright notice in the Description page of Project Settings.


#include "EnvironmentManager.h"
#include "Engine/World.h"
#include "EndlessRunnerGameMode.h"

#include "BPHandler/BPHanlder_SkySphere.h"
#include "BPHandler/BPHandler_Weather.h"

#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/Components/ExponentialHeightFogComponent.h"
#include "Runtime/Core/Public/Math/Color.h"
#include "EngineUtils.h"


// Sets default values
AEnvironmentManager::AEnvironmentManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    // Set Class Default Variables
    SunSpeedMultiplier = 1.f;
    InitialSunIntensity = 2.75f;
    SunBrightness = 350.f;

    CloudSpeed = 2.0f;
    CloudOpacity = 1.0f;

    FogDensity = 0.f;
    FogHeightFalloff = 0.2;
    FogMaxOpacity = 0.2;
    FogStartDistance = 1000.f;
    FogColor = FLinearColor();
    SunLightLerpPercentage = 0.15f;
}

// Called when the game starts or when spawned
void AEnvironmentManager::BeginPlay()
{
	Super::BeginPlay();

    // Register to GameMode
    RegisterToGameMode();

    if(GetWorld()){
        // Get Reference to SkySphereHandler
        // And Init SkySphereHandler
        for (TActorIterator<ABPHanlder_SkySphere> ActorItr(GetWorld(), ABPHanlder_SkySphere::StaticClass()); ActorItr; ++ActorItr){
            // Assuming that there is only one SegmentationManager
            SkySphereHandler = *ActorItr;
            UE_LOG(LogTemp, Warning, TEXT("found SkySphereHandler"));

            SkySphereHandler->LoadSkySphere(); // Getting access to the blueprint components in the scene
            SkySphereHandler->SetTimeOfDay(DayTime.Hours, DayTime.Minutes, DayTime.Seconds); // Setting initial time of day
            SkySphereHandler->UpdateSun(); // Call for Sun Update (Tempareture)
        }
        // Get Reference to WeatherHandler
        // And Init WeatherHandler
        for (TActorIterator<ABPHandler_Weather> ActorItr(GetWorld(), ABPHandler_Weather::StaticClass()); ActorItr; ++ActorItr){
            // Assuming that there is only one WeatherHandler
            WeatherHandler = *ActorItr;
            UE_LOG(LogTemp, Warning, TEXT("found WeatherHandler"));

            WeatherHandler->LoadWeatherHandler(); // Getting access to the blueprint components in the scene
        }

        // Get Reference to ExponentialHeightFog
        // And Init ExponetialHeightFog
        for (TActorIterator<AExponentialHeightFog> ActorItr(GetWorld(), AExponentialHeightFog::StaticClass()); ActorItr; ++ActorItr){
            // Assuming that there is only one ExponentialHeightFog
            ExponentialHeightFog = *ActorItr;
            UE_LOG(LogTemp, Warning, TEXT("found ExponentialHeightFog"));

            // Save initial FogColor Value
            FogColor = ExponentialHeightFog->GetComponent()->FogInscatteringColor;
        }
    }

    // Get Initial Sun Intensity
    InitialSunIntensity = SunLight->GetLightComponent()->Intensity;

}

// Called every frame
void AEnvironmentManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    if(IsSunMoving){
        MoveSun(DeltaTime);
        SetFogColorAccordingToSun();
    }

    if(IsUpdateEnvironment){ // Run interpolation of environment
        InterpolateEnvironmentChange();
        // Update the SkyLight cubemap during interpolation of the environment
        SkyLightRecaptureTimeAccum += DeltaTime;
        if(SkyLightRecaptureTimeAccum >= SkyLightRecaptureTime){
            SkySphereHandler->ReCaputreSkyLight();
            SkyLightRecaptureTimeAccum -= SkyLightRecaptureTime;
        }
    }
}

void AEnvironmentManager::MoveSun(float DeltaTime){
    // Add DeltaTime to daytime 
    UpdateTimeOfDay(DeltaTime);
    // Call daytime update for sun movement
    SkySphereHandler->SetTimeOfDay(DayTime.Hours, DayTime.Minutes, DayTime.Seconds);
    SkySphereHandler->UpdateSun();
}


void AEnvironmentManager::UpdateTimeOfDay(float DeltaTime){
    DayTime.Seconds += DeltaTime*SunSpeedMultiplier;
    // Update Seconds
    if(DayTime.Seconds > 60.f){
        DayTime.Minutes += 1;
        DayTime.Seconds -= 60.f;
        SkySphereHandler->ReCaputreSkyLight();
    } 
    // Update Minutes
    if(DayTime.Minutes > 59){
        DayTime.Hours += 1;
        DayTime.Minutes = 0;
    }
    // Update Hours
    if(DayTime.Hours > 23){
        DayTime.Hours = 0;
    }
}


void AEnvironmentManager::SetFogColorAccordingToSun(){
    FLinearColor fogColor = FMath::Lerp(FogColor, FLinearColor::MakeFromColorTemperature(SunLight->GetLightComponent()->Temperature), SunLightLerpPercentage);
    ExponentialHeightFog->GetComponent()->SetFogInscatteringColor(fogColor);
}


void AEnvironmentManager::SetNewEnvironment(FEnvironmentStruct& NewEnvironmentStruct, bool IsSetHard){
    // Set new environment variables
    IsSunMoving = NewEnvironmentStruct.IsSunMoving;
    DayTime = NewEnvironmentStruct.DayTime;
    SunSpeedMultiplier = NewEnvironmentStruct.SunSpeedMultiplier;

    // Set new interpolation timer
    InterpolationTime = NewEnvironmentStruct.InterpolationTime;

    // Set interpolation targtes
    SunIntensityTarget = NewEnvironmentStruct.SunIntenstiy;
    SkyLightIntensityTarget = NewEnvironmentStruct.SkyLightIntensity;
    SunBrightnessTarget = NewEnvironmentStruct.SunBrightness;
    FogDensityTarget = NewEnvironmentStruct.FogDenstiy;
    FogHeightFalloffTarget = NewEnvironmentStruct.FogHeightFalloff;
    FogMaxOpacityTarget = NewEnvironmentStruct.FogMaxOpacity;
    FogStartDistanceTarget = NewEnvironmentStruct.FogStartDistance;
    CloudOpacityTarget = NewEnvironmentStruct.CloudOpacity;
    CloudSpeedTarget = NewEnvironmentStruct.CloudSpeed;

    // Immediately Set TimeOfDay for Sun
    SkySphereHandler->SetTimeOfDay(DayTime.Hours, DayTime.Minutes, DayTime.Seconds);
    SkySphereHandler->UpdateSun();
    SkySphereHandler->ReCaputreSkyLight();
    SkySphereHandler->ReCaputreSkyLight();

    // Immediately Set Weather
    WeatherHandler->SetIsRaining(NewEnvironmentStruct.IsRaining);
    WeatherHandler->SetIsSnowing(NewEnvironmentStruct.IsSnowing);
    WeatherHandler->SetParticleBrightness(NewEnvironmentStruct.ParticleBrightness);
    WeatherHandler->SetParticleDensity(NewEnvironmentStruct.ParticleDensity);
    WeatherHandler->SetRainImpactDensity(NewEnvironmentStruct.RainImpactDenstiy);
    WeatherHandler->SetMaterialAdjustmentStrength(NewEnvironmentStruct.MaterialAdjustmentStrength);
    UE_LOG(LogTemp, Warning, TEXT("Set new weather environment..."));

    if(IsSetHard){ // Call for an immediate environment update
        UpdateEnvironment();

    } else{ // Call for environment change via interpolation
        // Check if there is a interpolation running
        if(IsUpdateEnvironment){ // if already true
            ReReadCurrentEnvironment();
        }

        // Set Interpolation StartTime
        InterpolationStartTime = GetWorld()->GetTimeSeconds();

        // Set UpdateEnvironment Flag
        IsUpdateEnvironment = true;
    }

}

/*
* Function that propagates updates of all environment components that should change over time(steps)
*/
void AEnvironmentManager::UpdateEnvironment(){
    // Update EnvironmentHandler variables
    FogDensity = FogDensityTarget;
    FogMaxOpacity = FogMaxOpacityTarget;
    FogStartDistance = FogStartDistanceTarget;
    FogHeightFalloff = FogHeightFalloffTarget;
    CloudOpacity = CloudOpacityTarget;
    CloudSpeed = CloudSpeedTarget;

    // Update Environment Components
    UE_LOG(LogTemp, Warning, TEXT("Sun Intentisty %f"), SunIntensityTarget);
    SkySphereHandler->SetSunIntensity(SunIntensityTarget);
    SkySphereHandler->SetSkylightIntensity(SkyLightIntensityTarget);
    SkySphereHandler->SetSunBrightness(SunBrightnessTarget);
    SkySphereHandler->SetCloudOpacity(CloudOpacity);
    SkySphereHandler->SetCloudSpeed(CloudSpeed);

    ExponentialHeightFog->GetComponent()->SetFogDensity(FogDensityTarget);
    ExponentialHeightFog->GetComponent()->SetFogMaxOpacity(FogMaxOpacityTarget);
    ExponentialHeightFog->GetComponent()->SetStartDistance(FogStartDistanceTarget);
    ExponentialHeightFog->GetComponent()->SetFogHeightFalloff(FogHeightFalloffTarget);

    // Recapture sky
    SkySphereHandler->ReCaputreSkyLight();
}

void AEnvironmentManager::InterpolateEnvironmentChange(){
    // Get current time in seconds
    float currTime = GetWorld()->GetTimeSeconds();
    // Calculate InterpolationPercentage
    float interpolationPercentage = (currTime+0.01f - InterpolationStartTime) / InterpolationTime;
    //UE_LOG(LogTemp, Log, TEXT("InterpolationPercentage: %f"), interpolationPercentage);
    // Set interpolated values
    SunBrightnessInterpolating = FMath::Lerp(SunBrightness, SunBrightnessTarget, interpolationPercentage);
    SkySphereHandler->SetSunBrightness(SunBrightnessInterpolating);
    SkySphereHandler->SetCloudOpacity(FMath::Lerp(CloudOpacity, CloudOpacityTarget, interpolationPercentage));
    SkySphereHandler->SetCloudSpeed(FMath::Lerp(CloudSpeed, CloudSpeedTarget, interpolationPercentage));

    ExponentialHeightFog->GetComponent()->SetFogDensity(FMath::Lerp(FogDensity, FogDensityTarget, interpolationPercentage));
    ExponentialHeightFog->GetComponent()->SetFogMaxOpacity(FMath::Lerp(FogMaxOpacity, FogMaxOpacityTarget, interpolationPercentage));
    ExponentialHeightFog->GetComponent()->SetStartDistance(FMath::Lerp(FogStartDistance, FogStartDistanceTarget, interpolationPercentage));
    ExponentialHeightFog->GetComponent()->SetFogHeightFalloff(FMath::Lerp(FogHeightFalloff, FogHeightFalloffTarget, interpolationPercentage));

    // Check if interpolation complete
    if(interpolationPercentage >= 1.f){
        UpdateEnvironment();
        IsUpdateEnvironment = false;
    }
}

void AEnvironmentManager::ReReadCurrentEnvironment(){
    SunBrightness = SunBrightnessInterpolating;

    FogDensity = ExponentialHeightFog->GetComponent()->FogDensity;
    FogMaxOpacity = ExponentialHeightFog->GetComponent()->FogMaxOpacity;
    FogStartDistance = ExponentialHeightFog->GetComponent()->StartDistance;
    FogHeightFalloff = ExponentialHeightFog->GetComponent()->FogHeightFalloff;
}

void AEnvironmentManager::RegisterToGameMode(){
    if(GetWorld()){
        if(GetWorld()->GetAuthGameMode()){
            AEndlessRunnerGameMode* gameModeCallback = (AEndlessRunnerGameMode*)GetWorld()->GetAuthGameMode();
            gameModeCallback->EnvironmentManager = this;
            UE_LOG(LogTemp, Warning, TEXT("EnvironmentManager: Registered to GameMode"));
        }
    }
}