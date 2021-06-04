// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

class ULevel;

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "EndlessRunner_GameInstance.generated.h"

/**
 * 
 */
UCLASS()
class ENDLESSRUNNER_API UEndlessRunner_GameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="EndlessRunner_GameInstance")
	TArray<FString> JsonSequencePaths = TArray<FString>();

};
