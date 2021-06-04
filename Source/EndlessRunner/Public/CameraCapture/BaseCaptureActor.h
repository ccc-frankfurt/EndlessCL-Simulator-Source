// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Runtime/Engine/Classes/Camera/PlayerCameraManager.h"
#include "Runtime/Engine/Classes/Components/SceneCaptureComponent2D.h"
#include "Runtime/Engine/Classes/Engine/TextureRenderTarget2D.h"
#include <Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h>

#include "Async.h"
#include "IImageWrapper.h"
#include "Containers/Queue.h"

#include "BaseCaptureActor.generated.h"

USTRUCT()
struct FRenderRequest{
    GENERATED_BODY()

    TArray<FColor> Image;
    FRenderCommandFence RenderFence;

    FRenderRequest(){
    }
};

USTRUCT()
struct FFloatRenderRequest{
    GENERATED_BODY()

    TArray<FFloat16Color> Image;
    FRenderCommandFence RenderFence;

    FFloatRenderRequest(){
    }
};

UCLASS()
class ENDLESSRUNNER_API ABaseCaptureActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseCaptureActor();

    int32 FrameWidth = 1240;
    int32 FrameHeight = 780;

    // First Player camer manager to attach our capture component to
    APlayerCameraManager* PlayerCameraManager = nullptr;
    // Capture Component will be our camera
    USceneCaptureComponent2D* CaptureComponent = nullptr;
    // Render Target holds frames captured by Capture Component
    UTextureRenderTarget2D* RenderTarget = nullptr;

    // PostProcessMaterial
    UMaterial* PostProcessMaterial = nullptr;

    FString ImagePrefix = "";
    FString ImageSubDir = "";
    FString ImageSequenceDir = "";
    FString ImageSaveDir = "";
    int32 ImageCounter = 0;

    int32 CurrCaptureFPS = 0; //DEBUG
    float DeltaTimeAccum = 0.f;

    TArray<FColor> Pixels;
    bool IsReadPixelsStarted = false;
    TQueue<int32> ReadPixelQueue;
    TQueue<FRenderCommandFence> RenderFenceQueue;

    TQueue<FRenderRequest*> RenderRequestQueue;
    TQueue<FFloatRenderRequest*> FloatRenderRequestQueue;

    FRenderCommandFence RenderCommandFence;

    FCriticalSection* NumActiveThreadsLockPtr = nullptr;
    int32* NumActiveThreadsPtr = nullptr;

    bool IsSegmentationCapture = false;
    bool IsDepthCapture = false;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

    void FollowPlayerCamera();

    FString GenerateFileName();
    FString ToStringWithLeadingZeros(int32 Integer, int32 MaxDigits);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

    void SetImageSequenceDir(FString NewDir);

    void CaptureFrame();
    void CaptureFrameFloat();

    void CaptureFrameNonBlocking();
    void CaptureFrameFloat16NonBlocking();

    void RunAsyncImageSaveTaks(TArray64<uint8> Image, FString ImageName,
                               int32 FrameWidth, int32 FrameHeight, int32 *ThreadCounter, FCriticalSection *ThreadCounterLock);

    void RunAsyncImageFloat16SaveTaks(TArray64<FFloat16Color> Image, FString ImageName,
                               int32 FrameWidth, int32 FrameHeight, int32 *ThreadCounter, FCriticalSection *ThreadCounterLock,
                               TSharedPtr<IImageWrapper> ImageWrapper);
	
};


/*
* *******************************************************
*/

class AsyncSaveImageToDiskTask : public FNonAbandonableTask
{
public:
    AsyncSaveImageToDiskTask(TArray64<uint8> Image, FString ImageName,
                             int32 FrameWidth, int32 FrameHeight, int32 *ThreadCounter, FCriticalSection* ThreadCounterLock);
    ~AsyncSaveImageToDiskTask();

    // Required by UE4
    FORCEINLINE TStatId GetStatId() const{
        RETURN_QUICK_DECLARE_CYCLE_STAT(AsyncSaveImageToDiskTask, STATGROUP_ThreadPoolAsyncTasks);
    }

protected:
    TArray<uint8> ImageCopy;
    FString FileName = "";

    //TSharedPtr<IImageWrapper> ImageWrapper;

    int32 FrameWidth = -1;
    int32 FrameHeight = -1;

    int32* ThreadCounter = nullptr;
    FCriticalSection* ThreadCounterLock = nullptr;

public:
    void DoWork();

};

/*
* *******************************************************
*/

class AsyncSaveImageFloat16ToDiskTask : public FNonAbandonableTask
{
public:
    AsyncSaveImageFloat16ToDiskTask(TArray64<FFloat16Color> Image, FString ImageName,
    int32 FrameWidth, int32 FrameHeight, int32 *ThreadCounter, FCriticalSection* ThreadCounterLock, TSharedPtr<IImageWrapper> ImageWrapper);
    ~AsyncSaveImageFloat16ToDiskTask();

    // Required by UE4
    FORCEINLINE TStatId GetStatId() const{
        RETURN_QUICK_DECLARE_CYCLE_STAT(AsyncSaveImageToDiskTask, STATGROUP_ThreadPoolAsyncTasks);
    }

protected:
    TArray<FFloat16Color> ImageCopy;
    FString FileName = "";
    TSharedPtr<IImageWrapper> ImageWrapperCallback;

    TSharedPtr<IImageWrapper> ImageWrapper;

    int32 FrameWidth = -1;
    int32 FrameHeight = -1;

    int32* ThreadCounter = nullptr;
    FCriticalSection* ThreadCounterLock = nullptr;

public:
    void DoWork();
};
