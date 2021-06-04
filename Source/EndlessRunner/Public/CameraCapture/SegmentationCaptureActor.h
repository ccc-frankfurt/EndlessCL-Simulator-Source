// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CameraCapture/BaseCaptureActor.h"
#include "SegmentationCaptureActor.generated.h"

/**
 * 
 */
UCLASS()
class ENDLESSRUNNER_API ASegmentationCaptureActor : public ABaseCaptureActor
{
	GENERATED_BODY()

public:
    ASegmentationCaptureActor();

protected:
    virtual void BeginPlay() override;
	
	
};
