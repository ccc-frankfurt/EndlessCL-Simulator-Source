// Fill out your copyright notice in the Description page of Project Settings.


#include "SegmentationManager.h"

#include "EndlessRunnerGameMode.h"

#include <Engine/World.h>
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "Engine.h"


// Sets default values
ASegmentationManager::ASegmentationManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASegmentationManager::BeginPlay()
{
	Super::BeginPlay();

    // Register to GameMode
    RegisterToGameMode();

    // Get NumFramesToCapture // TODO: Remove this
    NumFramesToCapture = 10000000;//SequenceTime * SequenceFPS;

    // Set initial capture delay
    DeltaTimeAccumulated = -InitialCaptureDelay;

    // Init InstanceSegmentationCounterDict
    InitInstSegCounters();

    // Setup CaptureComponents
    if(GetWorld()){
        // Init Reference to (Player-)Charakter
        //Charakter = (AEndlessRunnerCharacter*)UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

        // Init ImageCapture with default (0,0,0) transform
        if(UseImageCapture){
            ImageCapture = GetWorld()->SpawnActorDeferred<AImageCaptureActor>(AImageCaptureActor::StaticClass(), FTransform());
            ImageCapture->FrameWidth = CaptureResolutionWidth;
            ImageCapture->FrameHeight = CaptureResolutionHeight;
            ImageCapture->ImageSubDir = ColorDirName;
            ImageCapture->NumActiveThreadsPtr = &NumActiveThreads;
            ImageCapture->NumActiveThreadsLockPtr = &NumActiveThreadsLock;
            UGameplayStatics::FinishSpawningActor(ImageCapture, FTransform());
        }
        // Init SegmentationCapture
        if(UseSegmentationCapture){
            SegmentationCapture = GetWorld()->SpawnActorDeferred<ASegmentationCaptureActor>(ASegmentationCaptureActor::StaticClass(), FTransform());
            SegmentationCapture->FrameWidth = CaptureResolutionWidth;
            SegmentationCapture->FrameHeight = CaptureResolutionHeight;
            SegmentationCapture->ImageSubDir = SegDirName;
            SegmentationCapture->NumActiveThreadsPtr = &NumActiveThreads;
            SegmentationCapture->NumActiveThreadsLockPtr = &NumActiveThreadsLock;
            UGameplayStatics::FinishSpawningActor(SegmentationCapture, FTransform());
        }
        // Init NormalsCapture
        if(UseNormalsCapture){
            NormalsCapture = GetWorld()->SpawnActorDeferred<ANormalsCaptureActor>(ANormalsCaptureActor::StaticClass(), FTransform());
            NormalsCapture->FrameWidth = CaptureResolutionWidth;
            NormalsCapture->FrameHeight = CaptureResolutionHeight;
            NormalsCapture->ImageSubDir = NormalsDirName;
            NormalsCapture->NumActiveThreadsPtr = &NumActiveThreads;
            NormalsCapture->NumActiveThreadsLockPtr = &NumActiveThreadsLock;
            UGameplayStatics::FinishSpawningActor(NormalsCapture, FTransform());
        }
        // Init DepthCapture
        if(UseDepthCapture){
            DepthCapture = GetWorld()->SpawnActorDeferred<ADepthCaptureActor>(ADepthCaptureActor::StaticClass(), FTransform());
            DepthCapture->FrameWidth = CaptureResolutionWidth;
            DepthCapture->FrameHeight = CaptureResolutionHeight;
            DepthCapture->ImageSubDir = DepthDirName;
            UGameplayStatics::FinishSpawningActor(DepthCapture, FTransform());
        }
    }

    // Calculate TimeDilation needed as indicated by variables set
    CalculateTimeDilation();

    // Time Dilation
    UGameplayStatics::SetGlobalTimeDilation(GetWorld(), TimeDilation);
    UE_LOG(LogTemp, Log, TEXT("TimeDilation: %f"), TimeDilation);

    // Set RenderDeltaTime
    RenderDeltaTime = (1.f/CaptureFPS) * TimeDilation;
    UE_LOG(LogTemp, Warning, TEXT("RenderDeltaTime: %f"), RenderDeltaTime);

    // DEBUG ONLY AFTER THIS
    //EnableInput(GetWorld()->GetFirstPlayerController());
    //UInputComponent* InputComponent = this->InputComponent;
    //InputComponent->BindAction("CaptureFrame", IE_Released, this, &ASegmentationManager::Capture);

}

// Called every frame
void ASegmentationManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    // Check if sequence is finished
    if(NumFramesCaptured >= NumFramesToCapture){
        // Set SequenceComplete flag
        IsSequenceComplete = true;
        // Omit rest of Tick
        // FIXME: Remove this for haveing more than 1 sequence
        // if simulation ending
        if(GetWorld()){
            if(GetWorld()->GetAuthGameMode()){
                AEndlessRunnerGameMode* GameModeCallback = (AEndlessRunnerGameMode*)GetWorld()->GetAuthGameMode();
                GameModeCallback->QuitGame();
                return;
            }
        }
        return;
    } else{
        MessageTimeAccumulated += DeltaTime;
        if(MessageTimeAccumulated >= 1.f){
            // Display On Screen Message
            GEngine->AddOnScreenDebugMessage(-1, 0.98f, FColor::Green, FString::Printf(TEXT("Frames: %d, of %d"), NumFramesCaptured, NumFramesToCapture));
            MessageTimeAccumulated -= 1.f;
        }
        //UE_LOG(LogTemp,Log,TEXT("NumFramesCaptured: %d / %d"), NumFramesCaptured, NumFramesToCapture);
    }

    // Capture Gate Logic
    DeltaTimeAccumulated += DeltaTime;
    if(IsCaptureing){
        if(DeltaTimeAccumulated >= RenderDeltaTime){
            DebugFPS += 1;
            // Capture Call / Render Call
            Capture();
            // Increase counter on captured frames
            NumFramesCaptured += 1;
            // Update DeltaTimeAccumulated
            DeltaTimeAccumulated -= RenderDeltaTime;
        }
    }
}

/*
 * Load and Reset Manager for the next Sequence to be rendered
 */
void ASegmentationManager::LoadSequenceModule(FCaptureSettings CaptureSettings, int32 SequenceCounter){
    if(!GetWorld()){
        UE_LOG(LogTemp, Fatal, TEXT("SegmentationManager: LoadSequenceModule: No world found!"));
    }
    // Reset NumFramesCaptured
    NumFramesCaptured = 0;
    // Reset IsSequenceComplete
    IsSequenceComplete = false;
    // Reset InitalCaptureDelay
    DeltaTimeAccumulated = -InitialCaptureDelay;

    // Load CaptureComponent Settings
    IsInstanceSegmentation = CaptureSettings.IsInstanceSegmentation;
    //TODO: Initialize CaptureComponents according to these settings
    InitCaptureComponents(CaptureSettings);
    UE_LOG(LogTemp, Warning, TEXT("Initialized CaptureComponents"));
    // Set SubDirectoryIndex for this sequence (AFTER CaptureComponents are initialized)
    SetSequenceDirInCaptureComponents(SequenceCounter);

    // Load CaptureTime Settings
    // Overwrite CaptureFPS and SequenceFPS
    CaptureFPS = CaptureSettings.CaptureFPS;
    SequenceFPS = CaptureSettings.SequenceFPS;
    // Calculate TimeDilation needed as indicated by variables set
    CalculateTimeDilation();

    // Time Dilation
    UGameplayStatics::SetGlobalTimeDilation(GetWorld(), TimeDilation);
    UE_LOG(LogTemp, Log, TEXT("TimeDilation: %f"), TimeDilation);

    // Set RenderDeltaTime
    RenderDeltaTime = (1.f/CaptureFPS) * TimeDilation;
    UE_LOG(LogTemp, Warning, TEXT("RenderDeltaTime: %f"), RenderDeltaTime);
}

/*
*
*/
void ASegmentationManager::InitCaptureComponents(FCaptureSettings CaptureSettings){
    // Destroy CaptureComponents if existing
    if(ImageCapture){
        ImageCapture->Destroy();
        ImageCapture = nullptr;
    }
    if(SegmentationCapture){
        SegmentationCapture->Destroy();
        SegmentationCapture = nullptr;
    }
    if(NormalsCapture){
        NormalsCapture->Destroy();
        NormalsCapture = nullptr;
    }
    if(DepthCapture){
        DepthCapture->Destroy();
        DepthCapture = nullptr;
    }
    // Init new CaptureComponents
    int32 frameWidth = CaptureSettings.ImageWidth;
    int32 frameHeight = CaptureSettings.ImageHeight;
    if(CaptureSettings.CaptureModeSettings.Color){
        ImageCapture = GetWorld()->SpawnActorDeferred<AImageCaptureActor>(AImageCaptureActor::StaticClass(), FTransform());
        ImageCapture->FrameWidth = frameWidth;
        ImageCapture->FrameHeight = frameHeight;
        ImageCapture->ImageSubDir = ColorDirName;
        ImageCapture->NumActiveThreadsPtr = &NumActiveThreads;
        ImageCapture->NumActiveThreadsLockPtr = &NumActiveThreadsLock;
        UGameplayStatics::FinishSpawningActor(ImageCapture, FTransform());
    }
    // Init SegmentationCapture
    if(CaptureSettings.CaptureModeSettings.Segmentation){
        SegmentationCapture = GetWorld()->SpawnActorDeferred<ASegmentationCaptureActor>(ASegmentationCaptureActor::StaticClass(), FTransform());
        SegmentationCapture->FrameWidth = frameWidth;
        SegmentationCapture->FrameHeight = frameHeight;
        SegmentationCapture->ImageSubDir = SegDirName;
        SegmentationCapture->NumActiveThreadsPtr = &NumActiveThreads;
        SegmentationCapture->NumActiveThreadsLockPtr = &NumActiveThreadsLock;
        UGameplayStatics::FinishSpawningActor(SegmentationCapture, FTransform());
    }
    // Init NormalsCapture
    if(CaptureSettings.CaptureModeSettings.Normal){
        NormalsCapture = GetWorld()->SpawnActorDeferred<ANormalsCaptureActor>(ANormalsCaptureActor::StaticClass(), FTransform());
        NormalsCapture->FrameWidth = frameWidth;
        NormalsCapture->FrameHeight = frameHeight;
        NormalsCapture->ImageSubDir = NormalsDirName;
        NormalsCapture->NumActiveThreadsPtr = &NumActiveThreads;
        NormalsCapture->NumActiveThreadsLockPtr = &NumActiveThreadsLock;
        UGameplayStatics::FinishSpawningActor(NormalsCapture, FTransform());
    }
    // Init DepthCapture
    if(CaptureSettings.CaptureModeSettings.Depth){
        DepthCapture = GetWorld()->SpawnActorDeferred<ADepthCaptureActor>(ADepthCaptureActor::StaticClass(), FTransform());
        DepthCapture->FrameWidth = frameWidth;
        DepthCapture->FrameHeight = frameHeight;
        DepthCapture->ImageSubDir = DepthDirName;
        UGameplayStatics::FinishSpawningActor(DepthCapture, FTransform());
    }
    return;
}

/*
 * Look up SegmentationIndex for given ObjectName
 */
uint8 ASegmentationManager::GetSegmentationIndex(FString ObjName){
    if(IsInstanceSegmentation){ // Instance Segmentation
        if(InstanceSegmentationDict.Contains(ObjName)){
            // Get current instance counter for object class
            uint8 segIndex = InstSegCounterDict[ObjName];
            // Update object counter
            if((segIndex+1) >= InstanceSegmentationDict[ObjName].Max){
                InstSegCounterDict[ObjName] = InstanceSegmentationDict[ObjName].Min;
            } else {
                 InstSegCounterDict[ObjName] = segIndex + 1;
            }
            return segIndex;
        } else{
            UE_LOG(LogTemp, Error, TEXT("SegmentationManager: ObjName: %s could not be found in SegmentationDict!"), *ObjName);
            
        }
        return 0;

    } else { // Object Class Segmentation
        if(SegmentationDict.Contains(ObjName)){
            return SegmentationDict[ObjName];
        }else{
            UE_LOG(LogTemp, Error, TEXT("SegmentationManager: ObjName: %s could not be found in SegmentationDict!"), *ObjName);
        }
        return 0;
    }
    
}

/*
 * Calculate how much TimeDilation needs to be applied based on Capture and Sequence framerate
 */
void ASegmentationManager::CalculateTimeDilation(){
    TimeDilation = ((float)CaptureFPS / (float)SequenceFPS);
    UE_LOG(LogTemp, Warning, TEXT("TimeDilation: %f"), TimeDilation);
}

/*
 * Convert Frames back to Seconds in rendered Sequence time
 */
int32 ASegmentationManager::GetSecondsFromFrames(int32 NumFrames){
    return FMath::FloorToInt(NumFrames / SequenceFPS);
}

/*
* Sets the time dilation to 0.
*/
void ASegmentationManager::StopGameTime(){
    UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.0001f); // Cannot be set to full 0
}

/*
* Resets time dilation.
*/
void ASegmentationManager::TakeUpGameTime(){
    UGameplayStatics::SetGlobalTimeDilation(GetWorld(), TimeDilation);
}

/*
* Calls CaptureFrame on all selected CaptureComponents.
*/
void ASegmentationManager::Capture(){
    // Deploy frame capture commands
    if(ImageCapture){
        ImageCapture->CaptureFrameNonBlocking();
    }
    if(SegmentationCapture){
        SegmentationCapture->CaptureFrameNonBlocking();
    }
    if(NormalsCapture){
        NormalsCapture->CaptureFrameNonBlocking();
    }
    if(DepthCapture){
        DepthCapture->CaptureFrameFloat16NonBlocking();
        //DepthCapture->CaptureFrame();
    }
}

void ASegmentationManager::SetSequenceDirInCaptureComponents(int32 SequenceCounter){
    FString dirName = FString::FromInt(SequenceCounter) + "/";
    if(ImageCapture){
        ImageCapture->ImageSequenceDir = dirName;
    }
    if(SegmentationCapture){
        SegmentationCapture->ImageSequenceDir = dirName;
    }
    if(NormalsCapture){
        NormalsCapture->ImageSequenceDir = dirName;
    }
    if(DepthCapture){
        DepthCapture->ImageSequenceDir = dirName;
    }
}

/*
* Initialize the instance counters for instance segmentation
*/
void ASegmentationManager::InitInstSegCounters(){
    for (const TPair<FString, FMinMax>& pair : InstanceSegmentationDict){
        InstSegCounterDict.Add(pair.Key, pair.Value.Min);
    }
    return;
}



AComplexityManager* ASegmentationManager::GetComplexityManager(){
    if(GetWorld()){
        if(GetWorld()->GetAuthGameMode()){
            AEndlessRunnerGameMode* GameModeCallback = (AEndlessRunnerGameMode*)GetWorld()->GetAuthGameMode();
            AComplexityManager* complexityManager = GameModeCallback->ComplexityManager;
            return complexityManager;
        }
    }
    return nullptr;
}

void ASegmentationManager::RegisterToGameMode(){
    if(GetWorld()){
        if(GetWorld()->GetAuthGameMode()){
            AEndlessRunnerGameMode* gameModeCallback = (AEndlessRunnerGameMode*)GetWorld()->GetAuthGameMode();
            gameModeCallback->SegmentationManager = this;
            UE_LOG(LogTemp, Warning, TEXT("SegmentationManager: Registered to GameMode"));
        }
    }
}