// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"

#include "Common/CommonStructs.h"

#include "SequenceModule.generated.h"

/**
 * 
 */
UCLASS()
class ENDLESSRUNNER_API USequenceModule : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category="Sequence")
	FSequence Sequence;
};
