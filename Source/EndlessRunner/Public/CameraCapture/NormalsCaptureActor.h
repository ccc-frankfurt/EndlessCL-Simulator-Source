// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CameraCapture/BaseCaptureActor.h"
#include "NormalsCaptureActor.generated.h"

/**
 * 
 */
UCLASS()
class ENDLESSRUNNER_API ANormalsCaptureActor : public ABaseCaptureActor
{
	GENERATED_BODY()
	
public:
    ANormalsCaptureActor();

protected:
    virtual void BeginPlay() override;
	
	
};
