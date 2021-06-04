// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UIFunctions.generated.h"

/**
 * 
 */
UCLASS()
class ENDLESSRUNNER_API UUIFunctions : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/*
	* Code by https://forums.unrealengine.com/t/file-dialog-from-c-in-editorutilitywidget/134168
	*/
	//UFUNCTION(BlueprintCallable, Category="FilePicker")
	//static void GetFileDialog(const FString& DialogTitle, const FString& DefaultPath, const FString& FileTypes, TArray<FString>& OutFileNames);
};
