// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCaptureActor.h"
#include <Engine/World.h>
#include "Engine.h"
#include <Runtime/Engine/Public/HighResScreenshot.h>
#include <Runtime/Engine/Classes/Kismet/GameplayStatics.h>
#include <Runtime/Engine/Public/ShowFlags.h>
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "ImageUtils.h"

//static IImageWrapperModule &ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));

// Sets default values
ABaseCaptureActor::ABaseCaptureActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    // Set Class Variables
    ImageSaveDir = FPaths::ProjectSavedDir();

    // Init CaptureComponent
    CaptureComponent = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("CaptureComponent"));
    // Set Capture Component to be SceneRoot
    RootComponent = CaptureComponent;

    // Init RenderTarget
    RenderTarget = CreateDefaultSubobject<UTextureRenderTarget2D>(TEXT("RenderTarget"));

    // Do not init PostProcessMaterial!

}

// Called when the game starts or when spawned
void ABaseCaptureActor::BeginPlay()
{
	Super::BeginPlay();
	
    // Get CameraManager of player
    PlayerCameraManager = GetWorld()->GetFirstPlayerController()->PlayerCameraManager;

    // Register Capture Component with World
    // this is needed for some reason.. otherwise the whole thing explodes
    if(CaptureComponent){
        CaptureComponent->RegisterComponent();
    }

    // Setup the capture format
    RenderTarget = NewObject<UTextureRenderTarget2D>();

    // Setup Gamma
    RenderTarget->TargetGamma = GEngine->GetDisplayGamma();
    
    if(IsDepthCapture){ // Float
        RenderTarget->RenderTargetFormat = ETextureRenderTargetFormat::RTF_RGBA32f;
        RenderTarget->InitCustomFormat(FrameWidth, FrameHeight, PF_FloatRGBA, true);
        UE_LOG(LogTemp, Error, TEXT("Set Render Format for DepthCapture"));
    } else { // Uint
        RenderTarget->RenderTargetFormat = ETextureRenderTargetFormat::RTF_RGBA8; //8-bit color format
        RenderTarget->InitCustomFormat(FrameWidth, FrameHeight, PF_B8G8R8A8, true); // PF... disables HDR, which is most important since HDR gives gigantic overhead, and is not needed!
        UE_LOG(LogTemp, Warning, TEXT("Set Render Format for Color-Like-Captures"));
    }
    
    RenderTarget->bGPUSharedFlag = true; // demand buffer to be on GPU

    // Setup CaptureComponent
    CaptureComponent->CaptureSource = ESceneCaptureSource::SCS_FinalColorLDR; //FinalColor is the correct channel with Anti-Aliasing etc..
    CaptureComponent->TextureTarget = RenderTarget;
    CaptureComponent->FOVAngle = 90.f;
    CaptureComponent->TextureTarget->TargetGamma = GEngine->GetDisplayGamma(); //1.2f;  
    CaptureComponent->ShowFlags.SetTemporalAA(true);

}

// Called every frame
void ABaseCaptureActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    // Set own location and roatation to match player camera
    FollowPlayerCamera();

    // Ommit Tick for DepthCapture!
    //if(IsDepthCapture){
    //    return;
    //}

    // FPS Debug
    /*
    DeltaTimeAccum += DeltaTime;
    if(DeltaTimeAccum >= 1.f){
        UE_LOG(LogTemp, Log, TEXT("CurrCaptureFPS: %d, NumActiveThreads: %d"), CurrCaptureFPS, *NumActiveThreadsPtr);
        GEngine->AddOnScreenDebugMessage(-1, 0.98f, FColor::Green, FString::Printf(TEXT("CurrCaptureFPS: %d, NumThreads: %d"), CurrCaptureFPS, *NumActiveThreadsPtr));
        CurrCaptureFPS = 0;
        DeltaTimeAccum -= 1.f;
    }
    */
    // Read float pixels once RenderFence is ready
    if(!FloatRenderRequestQueue.IsEmpty()){
        FFloatRenderRequest* nextFloatRenderRequest = nullptr;
        FloatRenderRequestQueue.Peek(nextFloatRenderRequest);
        if(nextFloatRenderRequest){ // nullptr check
            bool isFenceDone = nextFloatRenderRequest->RenderFence.IsFenceComplete();
            if(isFenceDone){
                CurrCaptureFPS += 1;
                IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
                FString fileName = GenerateFileName();
                ImageCounter += 1;
                
                fileName += ".exr";
                static TSharedPtr<IImageWrapper> imageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::EXR);
                imageWrapper->SetRaw(nextFloatRenderRequest->Image.GetData(), nextFloatRenderRequest->Image.GetAllocatedSize(), FrameWidth, FrameHeight, ERGBFormat::RGBA, 16);
                const TArray64<uint8>& PngData = imageWrapper->GetCompressed(0);

                // Save to file in separate thread
                //FFileHelper::SaveArrayToFile(PngData, *fileName);
                //UE_LOG(LogTemp, Warning, TEXT("Stored Image: %s"), *fileName);
                RunAsyncImageSaveTaks(PngData, fileName, FrameWidth, FrameHeight, NumActiveThreadsPtr, NumActiveThreadsLockPtr);

                // Delete RenderRequest after saving
                // Remove request from queue
                FloatRenderRequestQueue.Pop();
                // Finally delete the renderRequest to prevent memory leakage
                delete nextFloatRenderRequest;
            }   
        }   
    }

    // Read pixels once RenderFence is ready
    if(!RenderRequestQueue.IsEmpty()){
        //UE_LOG(LogTemp, Log, TEXT("RenderRequestQueue not empty"));
        FRenderRequest* nextRenderRequest = nullptr;
        RenderRequestQueue.Peek(nextRenderRequest);
        if(nextRenderRequest){
            //UE_LOG(LogTemp, Log, TEXT("Peeked nextRenderReuquest"));
            if(nextRenderRequest->RenderFence.IsFenceComplete()){ //RenderCommandFence.IsFenceComplete()
                CurrCaptureFPS += 1;
                
                // Load the image wrapper module 
                IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));

                // Get file name
                //FString fileName = ImageSaveDir + ImageSubDir + ImagePrefix + ToStringWithLeadingZeros(ImageCounter, 5);
                FString fileName = GenerateFileName();
                ImageCounter += 1;

                // Run Async Save Image Thread
                *NumActiveThreadsPtr += 1;
                
                if(IsSegmentationCapture){ // Capture to .png
                    fileName += ".png";
                    static TSharedPtr<IImageWrapper> imageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG); //EImageFormat::PNG
                    imageWrapper->SetRaw(nextRenderRequest->Image.GetData(), nextRenderRequest->Image.GetAllocatedSize(), FrameWidth, FrameHeight, ERGBFormat::BGRA, 8);
                    const TArray64<uint8>& ImgData = imageWrapper->GetCompressed(5); //5 = PNG
                    RunAsyncImageSaveTaks(ImgData, fileName, FrameWidth, FrameHeight, NumActiveThreadsPtr, NumActiveThreadsLockPtr);
                } else { // Capture to .jpeg (this faster compression is needed)
                    fileName += ".jpeg";
                    static TSharedPtr<IImageWrapper> imageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::JPEG); //EImageFormat::PNG
                    imageWrapper->SetRaw(nextRenderRequest->Image.GetData(), nextRenderRequest->Image.GetAllocatedSize(), FrameWidth, FrameHeight, ERGBFormat::BGRA, 8);
                    const TArray64<uint8>& ImgData = imageWrapper->GetCompressed(0); 
                    RunAsyncImageSaveTaks(ImgData, fileName, FrameWidth, FrameHeight, NumActiveThreadsPtr, NumActiveThreadsLockPtr);
                }
                RenderRequestQueue.Pop();

                // Finally delete the renderRequest to prevent memory leakage
                delete nextRenderRequest;
            }
        }
    }
}

void ABaseCaptureActor::FollowPlayerCamera(){
    CaptureComponent->SetWorldLocation(PlayerCameraManager->GetCameraLocation());
    CaptureComponent->SetWorldRotation(PlayerCameraManager->GetCameraRotation());
}

FString ABaseCaptureActor::GenerateFileName(){
    FString filename = ImageSaveDir + ImageSubDir + ImageSequenceDir + ImagePrefix + ToStringWithLeadingZeros(ImageCounter, 6);
    return filename;
}

void ABaseCaptureActor::SetImageSequenceDir(FString NewDir){
    ImageSequenceDir = NewDir;
}

void ABaseCaptureActor::CaptureFrame(){
    UE_LOG(LogTemp, Warning, TEXT("Capture Started.."));
    CaptureComponent->CaptureScene();

    // Set name of file to be stored
    //FString fileName = ImageSaveDir + ImagePrefix + FString::FromInt(ImageCounter);
    FString fileName = ImageSaveDir + ImageSubDir + ImagePrefix + ToStringWithLeadingZeros(ImageCounter, 5);

    // Get image data from RenderTarget
    FTextureRenderTargetResource* renderTargetResource = RenderTarget->GameThread_GetRenderTargetResource();
    FReadSurfaceDataFlags readSurfaceDataFlags;
    readSurfaceDataFlags.SetLinearToGamma(false); // // Important  to set to "false" since we updated Gamma on our own!

    TArray<FColor> outBitMap;

    renderTargetResource->ReadPixels(outBitMap, readSurfaceDataFlags);

    //Hack to get Alpha Channel
    for (FColor& color : outBitMap){
        color.A = 255;
    }

    // Prepare for saving to disc
    static IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
    static TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG);
    ImageWrapper->SetRaw(outBitMap.GetData(), outBitMap.GetAllocatedSize(), FrameWidth, FrameHeight, ERGBFormat::BGRA, 8);
    const TArray64<uint8>& ImgData = ImageWrapper->GetCompressed();

    FFileHelper::SaveArrayToFile(ImgData, *fileName);
    UE_LOG(LogTemp, Warning, TEXT("image: %s"), *fileName);

    // Increase ImageCounter
    ImageCounter += 1;
}

void ABaseCaptureActor::CaptureFrameNonBlocking(){
    // Get resoucres
    FTextureRenderTargetResource* renderTargetResource = RenderTarget->GameThread_GetRenderTargetResource();

    struct FReadSurfaceContext{
        FRenderTarget* SrcRenderTarget;
        TArray<FColor>* OutData;
        FIntRect Rect;
        FReadSurfaceDataFlags Flags;
    };

    // Init new RenderRequest
    FRenderRequest* request = new FRenderRequest();

    // Setup GPU command
    FReadSurfaceContext readSurfaceContext = {
        renderTargetResource,
        &(request->Image),
        FIntRect(0, 0, renderTargetResource->GetSizeXY().X, renderTargetResource->GetSizeXY().Y),
        FReadSurfaceDataFlags(RCM_UNorm, CubeFace_MAX)
    };

    // Above 4.22 use this
    ENQUEUE_RENDER_COMMAND(SceneDrawCompletion)(
    [readSurfaceContext](FRHICommandListImmediate& RHICmdList){
        RHICmdList.ReadSurfaceData(
            readSurfaceContext.SrcRenderTarget->GetRenderTargetTexture(),
            readSurfaceContext.Rect,
            *readSurfaceContext.OutData,
            readSurfaceContext.Flags
        );
    });

    // Notify new task in queue
    RenderRequestQueue.Enqueue(request);

    // Set RenderCommandFence
    request->RenderFence.BeginFence();
    //UE_LOG(LogTemp, Log, TEXT("Started new RenderFence"));
}

/*
* This function is needed to save float16 images, such as depth data
* Derived from UnrealCV: https://github.com/unrealcv/unrealcv/blob/master/Source/UnrealCV/Private/Serialization.cpp
*/
void ABaseCaptureActor::CaptureFrameFloat(){
    UE_LOG(LogTemp, Warning, TEXT("Capture Started.."));
    CaptureComponent->CaptureScene();

    FString fileName = ImageSaveDir + ImageSubDir + ImagePrefix + ToStringWithLeadingZeros(ImageCounter, 5);

    // Get image data from RenderTarget
    FTextureRenderTargetResource* renderTargetResource = RenderTarget->GameThread_GetRenderTargetResource();

    TArray<FFloat16Color> outFBitMap;
    outFBitMap.AddUninitialized(FrameWidth*FrameHeight);

    renderTargetResource->ReadFloat16Pixels(outFBitMap);

    // EXR specifics
    if(outFBitMap.Num() == 0 || outFBitMap.Num() != FrameWidth*FrameHeight){
        UE_LOG(LogTemp, Error, TEXT("FloatCapture: outFBitMap has wrong dimensions..!"));
    }

    static IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
    static TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::EXR);
    ImageWrapper->SetRaw(outFBitMap.GetData(), outFBitMap.GetAllocatedSize(), FrameWidth, FrameHeight, ERGBFormat::RGBA, 16);
    const TArray64<uint8>& ExrData = ImageWrapper->GetCompressed();

    FFileHelper::SaveArrayToFile(ExrData, *fileName);
}

/*
* This function is needed to save float16 images, such as depth data
* Derived from UnrealCV: https://github.com/unrealcv/unrealcv/blob/master/Source/UnrealCV/Private/Serialization.cpp
*/
void ABaseCaptureActor::CaptureFrameFloat16NonBlocking(){
    // Get RenderContext
    //FTextureRenderTargetResource* renderTargetResource = CaptureComponent->GetCaptureComponent2D()->TextureTarget->GameThread_GetRenderTargetResource();
    //FTextureRenderTargetResource* renderTargetResource = RenderTarget->GameThread_GetRenderTargetResource();
    FTextureRenderTargetResource* renderTargetResource = CaptureComponent->TextureTarget->GameThread_GetRenderTargetResource();
    // Read the render target surface data back.	
	struct FReadSurfaceFloatContext
	{
		FRenderTarget* SrcRenderTarget;
		TArray<FFloat16Color>* OutData;
		FIntRect Rect;
		ECubeFace CubeFace;
	};

    // Init new RenderRequest
    FFloatRenderRequest* renderFloatRequest = new FFloatRenderRequest();

    // Setup GPU command
	FReadSurfaceFloatContext Context = {
		renderTargetResource,
		&(renderFloatRequest->Image),
        //&SurfaceData,
		FIntRect(0, 0, FrameWidth, FrameHeight),
		ECubeFace::CubeFace_MAX //no cubeface	
	};

	ENQUEUE_RENDER_COMMAND(ReadSurfaceFloatCommand)(
		[Context](FRHICommandListImmediate& RHICmdList) {
			RHICmdList.ReadSurfaceFloatData(
				Context.SrcRenderTarget->GetRenderTargetTexture(),
				Context.Rect,
				*Context.OutData,
				Context.CubeFace,
				0,
				0
				);
		});

    FloatRenderRequestQueue.Enqueue(renderFloatRequest);
    renderFloatRequest->RenderFence.BeginFence();
}


/*
void ABaseCaptureActor::CaptureFrameFloat16NonBlocking(){
    UE_LOG(LogTemp, Log, TEXT("Starting Float Capture Non Blocking..."));
    // Get resoucres
    FTextureRenderTargetResource* renderTargetResource = RenderTarget->GameThread_GetRenderTargetResource();
    // Define default cubeface
    ECubeFace CubeFace = CubeFace_PosX;

    // Read the render target surface data back.
    struct FReadSurfaceFloatContext
    {
        FRenderTarget* SrcRenderTarget;
        TArray<FFloat16Color>* OutData;
        FIntRect Rect;
        ECubeFace CubeFace;
    };
    UE_LOG(LogTemp, Log, TEXT("Setup ReadSurfaceStruct"));

    TArray<FFloat16Color> SurfaceData;
    FReadSurfaceFloatContext ReadSurfaceFloatContext =
    {
        renderTargetResource,
        &SurfaceData,
        FIntRect(0, 0, renderTargetResource->GetSizeXY().X, renderTargetResource->GetSizeXY().Y),
        CubeFace
    };
    UE_LOG(LogTemp, Log, TEXT("Going to setup render command.."));
    ENQUEUE_UNIQUE_RENDER_COMMAND_ONEPARAMETER(
        ReadSurfaceFloatCommand,
        FReadSurfaceFloatContext,Context,ReadSurfaceFloatContext,
    {
        RHICmdList.ReadSurfaceFloatData(
            Context.SrcRenderTarget->GetRenderTargetTexture(),
            Context.Rect,
            *Context.OutData,
            Context.CubeFace,
            0,
            0
            );
    });
    UE_LOG(LogTemp, Log, TEXT("Render command done.."));
    FlushRenderingCommands();

    // DEBUG
    const int32 ImageWidth = renderTargetResource->GetSizeXY().X;
    const int32 ImageHeight = renderTargetResource->GetSizeXY().Y;
    TArray<FFloat16Color> OutImageData;
    OutImageData.AddUninitialized(ImageWidth*ImageHeight);

    TArray<FFloat16Color>& OutImagePtr = OutImageData; // stupid hack

    // Copy the surface data into the output array.
    FFloat16Color* OutImageColors = reinterpret_cast< FFloat16Color* >((FFloat16Color*)&(OutImagePtr[0])); // ?

    // Cache width and height as its very expensive to call these virtuals in inner loop (never inlined)
    for (int32 Y = 0; Y < ImageHeight; Y++){
        FFloat16Color* SourceData = (FFloat16Color*) SurfaceData.GetData() + Y * ImageWidth;
        for (int32 X = 0; X < ImageWidth; X++){
            OutImageColors[ Y * ImageWidth + X ] = SourceData[X];
        }
    }

    FString fileName = ImageSaveDir + ImageSubDir + ImagePrefix + ToStringWithLeadingZeros(ImageCounter, 5);
    ImageCounter += 1;

    static IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
    static TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::EXR);

    ImageWrapper->SetRaw(OutImageData.GetData(), OutImageData.GetAllocatedSize(), FrameWidth, FrameHeight, ERGBFormat::RGBA, 16);
    //const TArray<uint8>& ExrData = ImageWrapper->GetCompressed();

    //FFileHelper::SaveArrayToFile(ExrData, *fileName);

    RunAsyncImageFloat16SaveTaks(OutImageData, fileName, FrameWidth, FrameHeight, NumActiveThreadsPtr, NumActiveThreadsLockPtr, ImageWrapper);

    
    // Notify new task in queue
    //RenderRequestQueue.Enqueue(request);

    // Set RenderCommandFence
    //request->RenderFence.BeginFence();
    //RenderCommandFence.BeginFence();
    //UE_LOG(LogTemp, Log, TEXT("Started new RenderFence"));
    

    UE_LOG(LogTemp, Log, TEXT("Rached End of Capture function"));
}
*/

FString ABaseCaptureActor::ToStringWithLeadingZeros(int32 Integer, int32 MaxDigits){
    FString result = FString::FromInt(Integer);
    int32 stringSize = result.Len();
    int32 stringDelta = MaxDigits - stringSize;
    if(stringDelta < 0){
        UE_LOG(LogTemp, Error, TEXT("MaxDigits of ImageCounter Overflow!"));
        return result;
    }
    //FIXME: Smarter function for this..
    FString leadingZeros = "";
    for(size_t i=0;i<stringDelta;i++){
        leadingZeros += "0";
    }
    result = leadingZeros + result;

    return result;
}

/*
* Function creating a new Async Thread that will store a copy of the current image to disk
*/

void ABaseCaptureActor::RunAsyncImageSaveTaks(TArray64<uint8> Image, FString ImageName, int32 FrameWidth, int32 FrameHeight, int32* ThreadCounter, FCriticalSection *ThreadCounterLock){
    UE_LOG(LogTemp, Warning, TEXT("Creating AsyncTask"));
    (new FAutoDeleteAsyncTask<AsyncSaveImageToDiskTask>(Image, ImageName, FrameWidth, FrameHeight, ThreadCounter, ThreadCounterLock))->StartBackgroundTask();
}

void ABaseCaptureActor::RunAsyncImageFloat16SaveTaks(TArray64<FFloat16Color> Image, FString ImageName, int32 FrameWidth, int32 FrameHeight, int32 *ThreadCounter, FCriticalSection *ThreadCounterLock, TSharedPtr<IImageWrapper> ImageWrapper){
    (new FAutoDeleteAsyncTask<AsyncSaveImageFloat16ToDiskTask>(Image, ImageName, FrameWidth, FrameHeight, ThreadCounter, ThreadCounterLock,
                                                               ImageWrapper))->StartBackgroundTask();
}

/*
 *********************************************
*/

AsyncSaveImageToDiskTask::AsyncSaveImageToDiskTask(
                         TArray64<uint8> Image, FString ImageName, int32 FrameWidth,
                         int32 FrameHeight, int32* ThreadCounter, FCriticalSection* ThreadCounterLock){
    ImageCopy = Image;
    FileName = ImageName;
    this->FrameWidth = FrameWidth;
    this->FrameHeight = FrameHeight;
    this->ThreadCounter = ThreadCounter;
    this->ThreadCounterLock = ThreadCounterLock;
}

AsyncSaveImageToDiskTask::~AsyncSaveImageToDiskTask(){
    UE_LOG(LogTemp, Warning, TEXT("AsyncTaskDone"));
}

void AsyncSaveImageToDiskTask::DoWork(){
    FFileHelper::SaveArrayToFile(ImageCopy, *FileName);
    UE_LOG(LogTemp, Warning, TEXT("Stored image: %s"), *FileName);

    //*ThreadCounter -= 1;
}

/*
 *********************************************
*/


AsyncSaveImageFloat16ToDiskTask::AsyncSaveImageFloat16ToDiskTask(
                         TArray64<FFloat16Color> Image, FString ImageName, int32 FrameWidth,
                         int32 FrameHeight, int32* ThreadCounter, FCriticalSection* ThreadCounterLock,
                         TSharedPtr<IImageWrapper> ImageWrapper){
    ImageCopy = Image;
    FileName = ImageName;
    ImageWrapperCallback = ImageWrapper;
    this->FrameWidth = FrameWidth;
    this->FrameHeight = FrameHeight;
    this->ThreadCounter = ThreadCounter;
    this->ThreadCounterLock = ThreadCounterLock;
}

AsyncSaveImageFloat16ToDiskTask::~AsyncSaveImageFloat16ToDiskTask(){
    //UE_LOG(LogTemp, Warning, TEXT("AsyncTaskDone"));
}

void AsyncSaveImageFloat16ToDiskTask::DoWork(){
    UE_LOG(LogTemp, Log, TEXT("Float16 Saving started..."));
    //ImageWrapperCallback->SetRaw(ImageCopy.GetData(), ImageCopy.GetAllocatedSize(), FrameWidth, FrameHeight, ERGBFormat::RGBA, 16);
    UE_LOG(LogTemp, Log, TEXT("Setting Raw was completed.."));
    const TArray64<uint8>& ExrData = ImageWrapperCallback->GetCompressed();
    UE_LOG(LogTemp, Log, TEXT("Compressing data was completed.."));

    FFileHelper::SaveArrayToFile(ExrData, *FileName);
    UE_LOG(LogTemp, Log, TEXT("Float16 Saving ended..."));
}
