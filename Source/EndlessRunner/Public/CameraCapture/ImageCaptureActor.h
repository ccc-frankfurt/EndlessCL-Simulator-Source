// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CameraCapture/BaseCaptureActor.h"
#include "ImageCaptureActor.generated.h"

/**
 * 
 */
UCLASS()
class ENDLESSRUNNER_API AImageCaptureActor : public ABaseCaptureActor
{
	GENERATED_BODY()

public:
    AImageCaptureActor();
protected:
    virtual void BeginPlay() override;
	
};
