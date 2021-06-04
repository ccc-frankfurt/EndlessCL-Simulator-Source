// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CameraCapture/BaseCaptureActor.h"
#include "DepthCaptureActor.generated.h"

/**
 * 
 */
UCLASS()
class ENDLESSRUNNER_API ADepthCaptureActor : public ABaseCaptureActor
{
	GENERATED_BODY()
public:
    ADepthCaptureActor();

protected:
    virtual void BeginPlay() override;
	
};
