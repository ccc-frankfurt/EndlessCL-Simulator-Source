// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BPHandler_Weather.generated.h"

UCLASS()
class ENDLESSRUNNER_API ABPHandler_Weather : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABPHandler_Weather();

	// BP Actor controlling the scene weather
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
    AActor* WeatherHandler = nullptr;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintImplementableEvent, Category="BPHandlerSkySphere")
	void LoadWeatherHandler();

	UFUNCTION(BlueprintImplementableEvent, Category="BPHandlerSkySphere")
	void SetIsRaining(bool IsRaining);

	UFUNCTION(BlueprintImplementableEvent, Category="BPHandlerSkySphere")
	void SetIsSnowing(bool IsSnowing);

	UFUNCTION(BlueprintImplementableEvent, Category="BPHandlerSkySphere")
	void SetParticleBrightness(float ParticleBrightenss);

	UFUNCTION(BlueprintImplementableEvent, Category="BPHandlerSkySphere")
	void SetParticleDensity(float ParticleDensity);
	
	UFUNCTION(BlueprintImplementableEvent, Category="BPHandlerSkySphere")
	void SetRainImpactDensity(float RainImpactDensity);

	UFUNCTION(BlueprintImplementableEvent, Category="BPHandlerSkySphere")
	void SetMaterialAdjustmentStrength(float MaterialAdjustmentStrength);

};
