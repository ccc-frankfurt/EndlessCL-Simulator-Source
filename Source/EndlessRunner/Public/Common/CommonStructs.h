// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/DataTable.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CommonStructs.generated.h"

USTRUCT()
struct FMinMax{
    GENERATED_BODY()
    
    UPROPERTY(EditAnywhere)
    uint8 Min;

    UPROPERTY(EditAnywhere)
    uint8 Max;

    FMinMax(){
        Min = 0;
        Max = 0;
    }

    FMinMax(uint8 Minimum, uint8 Maximum){
        Min = Minimum;
        Max = Maximum;
    }
};

USTRUCT()
struct FIMinMax{
    GENERATED_BODY()
    
    UPROPERTY(EditAnywhere)
    int32 Min;

    UPROPERTY(EditAnywhere)
    int32 Max;

    FIMinMax(){
        Min = 0;
        Max = 0;
    }

    FIMinMax(int32 Minimum, int32 Maximum){
        Min = Minimum;
        Max = Maximum;
    }
};

USTRUCT()
struct FMinMaxF{
    GENERATED_BODY()
    
    UPROPERTY(EditAnywhere)
    int32 Min;

    UPROPERTY(EditAnywhere)
    int32 Max;

    FMinMaxF(){
        Min = 0;
        Max = 0;
    }

    FMinMaxF(float Minimum, float Maximum){
        Min = Minimum;
        Max = Maximum;
    }
};


USTRUCT()
struct FDayTime{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere)
    int32 Hours;

    UPROPERTY(EditAnywhere)
    int32 Minutes;

    UPROPERTY(EditAnywhere)
    float Seconds;
    
    FDayTime(){
        Hours = 12;
        Minutes = 0;
        Seconds = 0.f;
    }
};


USTRUCT()
struct FObjectApperanceStruct{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere)
    bool Building;
    UPROPERTY(EditAnywhere)
    bool Tree;
    UPROPERTY(EditAnywhere)
    bool Car;
    UPROPERTY(EditAnywhere)
    bool People;
    UPROPERTY(EditAnywhere)
    bool Streetlamp;

    FObjectApperanceStruct(){
        Building = true;
        Tree = true;
        Car = true;
        People = true;
        Streetlamp = true;
    }
};

USTRUCT()
struct FMaterialActivationStruct{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere)
    bool UseAlbedo;
    UPROPERTY(EditAnywhere)
    bool UseNormals;
    UPROPERTY(EditAnywhere)
    bool UseRoughness;
    UPROPERTY(EditAnywhere)
    bool UseMetallic;
    
    //UPROPERTY(EditAnywhere)
    //bool UseSpecular;
    //UPROPERTY(EditAnywhere)
    //bool UseDisplacement;
    //UPROPERTY(EditAnywhere)
    //bool UseAmbientOcc;

    FMaterialActivationStruct(){
        UseAlbedo = true;
        UseNormals = true;
        UseRoughness = true;
        UseMetallic = true;
        //UseSpecular = true;
        //UseDisplacement = false;
        //UseAmbientOcc = true;
    }

    bool operator==(const FMaterialActivationStruct& other){
        if(this->UseAlbedo != other.UseAlbedo){
            return false;
        } else if(this->UseNormals != other.UseNormals){
            return false;
        } else if(this->UseRoughness != other.UseRoughness){
            return false;
        } else if(this->UseMetallic != other.UseMetallic){
            return false;
        } 
        return true;
    }
};

USTRUCT()
struct FEnvironmentStruct{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere)
    float SunIntenstiy;
    UPROPERTY(EditAnywhere)
    float SkyLightIntensity;
    //UPROPERTY(EditAnywhere)
    bool IsSunMoving;
    UPROPERTY(EditAnywhere)
    FDayTime DayTime;
    //UPROPERTY(EditAnywhere)
    float SunSpeedMultiplier;
    //UPROPERTY(EditAnywhere)
    float SunBrightness;

    // Time the linear interpolation takes (in seconds)
    UPROPERTY(EditAnywhere)
    float InterpolationTime;
    UPROPERTY(EditAnywhere)
    float CloudOpacity;
    //UPROPERTY(EditAnywhere)
    float CloudSpeed;
    UPROPERTY(EditAnywhere)
    float FogDenstiy;
    //UPROPERTY(EditAnywhere)
    float FogStartDistance;
    //UPROPERTY(EditAnywhere)
    float FogHeightFalloff;
    UPROPERTY(EditAnywhere)
    float FogMaxOpacity;

    UPROPERTY(EditAnywhere)
    bool IsRaining;
    UPROPERTY(EditAnywhere)
    bool IsSnowing;
    //UPROPERTY(EditAnywhere)
    float ParticleBrightness;
    UPROPERTY(EditAnywhere)
    float ParticleDensity;
    UPROPERTY(EditAnywhere)
    float RainImpactDenstiy;
    UPROPERTY(EditAnywhere)
    float MaterialAdjustmentStrength;

    FEnvironmentStruct(){
        SunIntenstiy = 9.6f;
        SkyLightIntensity = 2.f;
        IsSunMoving = false;
        SunSpeedMultiplier = 50.f;
        SunBrightness = 50.f;

        InterpolationTime = 1.f; // in seconds
        CloudOpacity = 1.0;
        CloudSpeed = 2.0;
        FogDenstiy = 0.f;
        FogHeightFalloff = 0.2;
        FogMaxOpacity = 0.2;
        FogStartDistance = 0.f;

        IsRaining = false;
        IsSnowing = false;
        ParticleBrightness = 0.2;
        ParticleDensity = 500;
        RainImpactDenstiy = 50;
        MaterialAdjustmentStrength = 1.25;
    }
};


USTRUCT()
struct FEnvironmentSettingStruct{
    GENERATED_BODY()
    
    UPROPERTY(EditAnywhere)
    FEnvironmentStruct EnvironmentStruct;
    UPROPERTY(EditAnywhere)
    FMaterialActivationStruct ShaderStruct;
    UPROPERTY(EditAnywhere)
    FObjectApperanceStruct ObjectApperanceStruct;

    FEnvironmentSettingStruct(){
    }
};

USTRUCT()
struct FCaptureModeStruct{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere)
    bool Color;
    UPROPERTY(EditAnywhere)
    bool Segmentation;
    UPROPERTY(EditAnywhere)
    bool Normal;
    UPROPERTY(EditAnywhere)
    bool Depth;
    FCaptureModeStruct(){
        Color = true;
        Segmentation = true;
        Normal = false;
        Depth = false;
    }
};

USTRUCT()
struct FCaptureSettings{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere)
    uint32 ImageWidth;
    UPROPERTY(EditAnywhere)
    uint32 ImageHeight;
    UPROPERTY(EditAnywhere)
    uint32 CaptureFPS;
    UPROPERTY(EditAnywhere)
    uint32 SequenceFPS;
    UPROPERTY(EditAnywhere)
    bool IsInstanceSegmentation;
    UPROPERTY(EditAnywhere)
    FCaptureModeStruct CaptureModeSettings;
    FCaptureSettings(){
        ImageWidth = 920;
        ImageHeight = 480;
        CaptureFPS = 5;
        SequenceFPS = 5;
        IsInstanceSegmentation = true;
    }
};

USTRUCT()
struct FStreetSampling{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere)
    float TilesToNextCurveMean = 4.f;
    UPROPERTY(EditAnywhere)
    float  TilesToNextCurveStddev = 0.45;
    FStreetSampling(){
    }
};

USTRUCT()
struct FObjectNumberSettings{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere)
    TMap<FString, FMinMax> ObjectSamplingNumbers;
    FObjectNumberSettings(){
        ObjectSamplingNumbers.Add("Human", FMinMax(0, 3));
        ObjectSamplingNumbers.Add("Tree", FMinMax(0, 3));
        ObjectSamplingNumbers.Add("Building", FMinMax(2, 2));
        ObjectSamplingNumbers.Add("Streetlamp", FMinMax(0, 1));
        ObjectSamplingNumbers.Add("Vehicle", FMinMax(0, 2));
    }
};

USTRUCT()
struct FSubSequence{
    GENERATED_BODY()
    
    UPROPERTY(EditAnywhere)
    int32 NumberOfTiles = 10;
    UPROPERTY(EditAnywhere)
    FStreetSampling StreetSampling;
    UPROPERTY(EditAnywhere)
    FEnvironmentStruct EnvironmentSettings;
    UPROPERTY(EditAnywhere)
    FMaterialActivationStruct MaterialSettings;
    UPROPERTY(EditAnywhere)
    FObjectApperanceStruct ObjectPresence;

    UPROPERTY(EditAnywhere)
    TMap<FString, float> CategoricalSamplingWeights;

    UPROPERTY(EditAnywhere)
    TMap<FString, FMinMax> ObjectSamplingNumbers;

    FSubSequence(){
        ObjectSamplingNumbers.Add("Human", FMinMax(0, 3));
        ObjectSamplingNumbers.Add("Tree", FMinMax(0, 3));
        ObjectSamplingNumbers.Add("Building", FMinMax(2, 2));
        ObjectSamplingNumbers.Add("Streetlamp", FMinMax(0, 1));
        ObjectSamplingNumbers.Add("Vehicle", FMinMax(0, 2));

        StreetSampling.TilesToNextCurveMean = 4.f;
        StreetSampling.TilesToNextCurveStddev = 0.45;
    }
};

USTRUCT()
struct FSequence{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere)
    int32 RandomSeed = 42;

    // Size of the TileBuffer
    UPROPERTY(EditAnywhere)
    uint32 NumberConcurrentTiles = 7;

    UPROPERTY(EditAnywhere)
    FCaptureSettings CaptureSettings;

    UPROPERTY(EditAnywhere)
    TArray<FSubSequence> SubSequenceSettings;

    FSequence(){
    }
};

UCLASS()
class ENDLESSRUNNER_API ACommonStructs : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACommonStructs();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	
};
