// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

class UBaseSequenceModule;
class AComplexityManager;

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CameraCapture/BaseCaptureActor.h"
#include "CameraCapture/ImageCaptureActor.h"
#include "CameraCapture/SegmentationCaptureActor.h"
#include "CameraCapture/NormalsCaptureActor.h"
#include "CameraCapture/DepthCaptureActor.h"

#include "Containers/Queue.h"

#include "Common/CommonStructs.h"

#include "EndlessRunnerCharacter.h"
#include "SegmentationManager.generated.h"

UCLASS()
class ENDLESSRUNNER_API ASegmentationManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASegmentationManager();

    // Whethere instance segmentation is active. This changes the interpretation of the segmentation dict
    UPROPERTY(EditAnywhere, Category="SegmentationIndices")
    bool IsInstanceSegmentation = false;
    
    UPROPERTY(EditAnywhere,  Category="SegmentationIndices")
    TMap<FString, uint8> SegmentationDict = TMap<FString, uint8>();

    UPROPERTY(EditAnywhere,  Category="SegmentationIndices")
    TMap<FString, FMinMax> InstanceSegmentationDict = TMap<FString, FMinMax>();

    // Whethere to capture images
    UPROPERTY(EditAnywhere, Category="Capture Directories")
    bool IsCaptureing = false;

    // Directory name for images
    UPROPERTY(EditAnywhere, Category="Capture Directories")
    FString ColorDirName = "Color/";
    UPROPERTY(EditAnywhere, Category="Capture Directories")
    FString SegDirName = "Seg/";
    UPROPERTY(EditAnywhere, Category="Capture Directories")
    FString NormalsDirName = "Normals/";
    UPROPERTY(EditAnywhere, Category="Capture Directories")
    FString DepthDirName = "Depth/";

    // The FPS rate of the rendered sequence
    UPROPERTY(EditAnywhere, Category="CaptureSettings")
    int32 SequenceFPS = 30;

    // The FPS rate at which images are captured
    UPROPERTY(EditAnywhere, Category="CaptureSettings")
    int32 CaptureFPS = 10;

    // The time of a sequence in Seconds
    //UPROPERTY(EditAnywhere, Category="CaptureSettings")
    int32 SequenceTime = 10000;

    // Delay until capture is initially fired in seconds
    UPROPERTY(EditAnywhere, Category="CaptureSettings")
    float InitialCaptureDelay = 2.f;

    // (Real-)Time between each capture call
    //UPROPERTY(EditAnywhere, Category="CaptureSettings")
    float RenderTimePerFrame = 1.f;

    UPROPERTY(EditAnywhere, Category="CaptureSettings")
    float CaptureResolutionWidth = 1240.f;

    UPROPERTY(EditAnywhere, Category="CaptureSettings")
    float CaptureResolutionHeight = 720.f;

    UPROPERTY(EditAnywhere, Category="CaptureActor")
    bool UseImageCapture = true;
    UPROPERTY(EditAnywhere, Category="CaptureActor")
    bool UseSegmentationCapture = true;
    UPROPERTY(EditAnywhere, Category="CaptureActor")
    bool UseNormalsCapture = false;
    UPROPERTY(EditAnywhere, Category="CaptureActor")
    bool UseDepthCapture = false;

    AImageCaptureActor* ImageCapture = nullptr;
    ASegmentationCaptureActor* SegmentationCapture = nullptr;
    ANormalsCaptureActor* NormalsCapture = nullptr;
    ADepthCaptureActor* DepthCapture = nullptr;

    FCriticalSection NumActiveThreadsLock;
    int32 NumActiveThreads = 0;

    int32 NumFramesToCapture = 0;
    int32 NumFramesCaptured = 0;

    bool IsSequenceComplete = false;

private:
    AEndlessRunnerCharacter* Charakter = nullptr;

    TMap<FString, uint8> InstSegCounterDict = TMap<FString, uint8>(); // mapping object name to current object instance counter

    float TimeDilation = 1.f;
    float RenderDeltaTime = 1.f; // time between captures
    bool doTakeUpGameTime = false; // whethere to trigger TakeUpGameTime()

    float DeltaTimeAccumulated = 0.f; // summed delta time to next capture
    float MessageTimeAccumulated = 0.f; // summed delta time to next progress message

    float DebugDeltaAccum = 0.f;
    int32 DebugFPS = 0;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

    void RegisterToGameMode();

    void InitCaptureComponents(FCaptureSettings CaptureSettings);
    void SetSequenceDirInCaptureComponents(int32 SequenceCounter);

    void CalculateTimeDilation();
    void InitInstSegCounters();

    int32 GetSecondsFromFrames(int32 NumFrames);

    void StopGameTime();
    void TakeUpGameTime();

    AComplexityManager* GetComplexityManager();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

    void LoadSequenceModule(FCaptureSettings CaptureSettings, int32 SequenceCounter);
    

    uint8 GetSegmentationIndex(FString ObjName);

    void Capture();
	
};
