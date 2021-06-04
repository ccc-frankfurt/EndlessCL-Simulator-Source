// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

class ABPHandler_Weather;
class ABPHanlder_SkySphere;

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <Runtime/Engine/Classes/Components/DirectionalLightComponent.h>
#include "Runtime/Engine/Classes/Engine/DirectionalLight.h"

//#include "BPHandler/BPHanlder_SkySphere.h"

#include "Runtime/Engine/Classes/Engine/ExponentialHeightFog.h"
#include "Runtime/Engine/Classes/Curves/CurveVector.h"

#include "ComplexityManager.h"

#include "Common/CommonStructs.h"

#include "EnvironmentManager.generated.h"

UCLASS()
class ENDLESSRUNNER_API AEnvironmentManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnvironmentManager();

    ABPHanlder_SkySphere* SkySphereHandler = nullptr;
    ABPHandler_Weather* WeatherHandler = nullptr;
    AExponentialHeightFog* ExponentialHeightFog = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sun")
    ADirectionalLight* SunLight;
    float InitialSunIntensity;

    UPROPERTY(EditAnywhere, Category="Sun")
    FDayTime DayTime;
    UPROPERTY(EditAnywhere, Category="Sun")
    bool IsSunMoving = false;
    UPROPERTY(EditAnywhere, Category="Sun")
    bool AllowNightTime = false;
    UPROPERTY(EditAnywhere, Category="Sun")
    float SunSpeedMultiplier;
    UPROPERTY(EditAnywhere, Category="Sun")
    float SunBrightness;
    UPROPERTY(EditAnywhere, Category="Sun")
    UCurveVector* SunCycleCurve;
    UPROPERTY(EditAnywhere, Category="Sun")
    float SunStartPercentage = 0.5f;

    UPROPERTY(EditAnywhere, Category="Clouds")
    float CloudSpeed;
    UPROPERTY(EditAnywhere, Category="Clouds")
    float CloudOpacity;

    UPROPERTY(EditAnywhere, Category="Fog")
    float FogDensity;
    UPROPERTY(EditAnywhere, Category="Fog")
    float FogStartDistance;
    UPROPERTY(EditAnywhere, Category="Fog")
    float FogHeightFalloff;
    UPROPERTY(EditAnywhere, Category="Fog")
    float FogMaxOpacity;

    FLinearColor FogColor;
    float SunLightLerpPercentage;

private:
    bool IsUpdateEnvironment = false;
    float InterpolationStartTime = 0.f; // in seconds
    float InterpolationTime = 10.f; // in seconds 
    // Time to recaputre skylight during environment interpolation
    UPROPERTY(EditAnywhere, Category="EnvironmentInterpolation")
    float SkyLightRecaptureTime = 0.5f; // in seconds
    float SkyLightRecaptureTimeAccum = 0.f;

    // Sun
    float SunInterpolationPercentage;
    float SunIntenstiy;
    float SkyLightIntensity;

    // Target values for interpolation
    float SunIntensityTarget;
    float SkyLightIntensityTarget;
    float SunBrightnessTarget;
    float FogDensityTarget;
    float FogStartDistanceTarget;
    float FogHeightFalloffTarget;
    float FogMaxOpacityTarget;
    float CloudOpacityTarget;
    float CloudSpeedTarget;

    // Interpolating values
    float SunBrightnessInterpolating;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

    void RegisterToGameMode();

    void MoveSun(float DeltaTime);

    void SetFogColorAccordingToSun();

    void GetNextFogTarget();

    void UpdateEnvironment();
    void UpdateLighting();

    void InterpolateEnvironmentChange();

    void ReReadCurrentEnvironment();

    void UpdateTimeOfDay(float DeltaTime);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

    void SetNewEnvironment(FEnvironmentStruct &NewEnvironmentStruct, bool IsSetHard=false);
	
};
