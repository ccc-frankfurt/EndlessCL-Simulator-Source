// Fill out your copyright notice in the Description page of Project Settings.


#include "UIFunctions.h"

//#include "DesktopPlatform/Public/IDesktopPlatform.h"
//#include "DesktopPlatform/Public/DesktopPlatformModule.h"
//#include "Engine/GameEngine.h"
//#include "EngineGlobals.h"
//#include "Engine/GameViewportClient.h"
//#include "Framework/Application/SlateApplication.h"

/*
void UUIFunctions::GetFileDialog(const FString& DialogTitle, const FString& DefaultPath, const FString& FileTypes, TArray<FString>& OutFileNames){
    
    if(GEngine){
        if(GEngine->GameViewport){

            void *ParentWindowHandle = GEngine->GameViewport->GetWindow()->GetNativeWindow()->GetOSWindowHandle();
            //const void *ParentWindowHandle = FSlateApplication::Get().FindBestParentWindowHandleForDialogs(nullptr);
            //IDesktopPlatform *DesktopPlatform = FSlateApplication::Get().FindBestParentWindowHandleForDialogs(nullptr)
            IDesktopPlatform *DesktopPlatform = FDesktopPlatformModule::Get();
            if(DesktopPlatform){ // nullptr check
                uint32 SelectionFlag = 0; //A value of 0 represents single file selection while a value of 1 represents multiple file selection
                DesktopPlatform->OpenFileDialog(ParentWindowHandle, DialogTitle, DefaultPath, FString(""), FileTypes, SelectionFlag, OutFileNames);
            }
        }
    }
    
}
*/