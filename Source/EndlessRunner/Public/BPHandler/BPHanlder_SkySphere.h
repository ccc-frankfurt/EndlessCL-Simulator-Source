// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

class ASkyLight;
class ADirectionalLight;

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BPHanlder_SkySphere.generated.h"

UCLASS()
class ENDLESSRUNNER_API ABPHanlder_SkySphere : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABPHanlder_SkySphere();

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    AActor* SkySphere = nullptr;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    AActor* SunPosition = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    ASkyLight* SkyLight = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    ADirectionalLight* SunLight = nullptr;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

    UFUNCTION(BlueprintImplementableEvent, Category="BPHandlerSkySphere")
    void LoadSkySphere();
	
    UFUNCTION(BlueprintImplementableEvent, Category="BPHandlerSkySphere")
    void SetSunBrightness(float Brightness);

    UFUNCTION(BlueprintImplementableEvent, Category="BPHandlerSkySphere")
    void SetSunIntensity(float Intesity);

    UFUNCTION(BlueprintImplementableEvent, Category="BPHandlerSkySphere")
    void SetSkylightIntensity(float Intensity);

    UFUNCTION(BlueprintImplementableEvent, Category="BPHandlerSkySphere")
    void UpdateSun();

    UFUNCTION(BlueprintImplementableEvent, Category="BPHandlerSkySphere")
    void SetCloudSpeed(float CloudSpeed);

    UFUNCTION(BlueprintImplementableEvent, Category="BPHandlerSkySphere")
    void SetCloudOpacity(float CloudOpacity);

    UFUNCTION(BlueprintImplementableEvent, Category="BPHandlerSkySphere")
    void SetTimeOfDay(int32 Houre, int32 Minute, int32 Seconds);

    UFUNCTION(BlueprintImplementableEvent, Category="BPHandlerSkySphere")
    void ReCaputreSkyLight();
};
