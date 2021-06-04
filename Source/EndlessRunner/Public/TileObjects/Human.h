// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

class ASegmentationManager;
class ARandomnessManager;
class AComplexityManager;

class USplineComponent;

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Human.generated.h"

UCLASS()
class ENDLESSRUNNER_API AHuman : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AHuman();

    float WalkSpeed = 120.f;

    // Speed for animation blendpsace
    UPROPERTY(BlueprintReadWrite, Category="Human")
    float Speed = 0.f;

    // This abuses float in animation blendspace to specify the 'type' of motion that shall be used
    UPROPERTY(BlueprintReadWrite, Category="Human")
    float MotionType = 0.f;

    float LastTargetSplinePercentage = 0.f;

    USplineComponent* GuidingSpline = nullptr;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

    ASegmentationManager* GetSegmentationManager();
    ARandomnessManager* GetRandomnessManager();
    AComplexityManager* GetComplexityManager();

    void CalculateNextWalkTargetOnSpline();

    float GetDistanceAlongSplineClosesToWorldLocaiton(USplineComponent* Spline, FVector WorldLocation);

    void SetupMaterial();

private:
    bool IsWalkUpLine = false; // whethere moving in spline direction or not

    FVector WalkTarget = FVector::ZeroVector;
    FVector WalkDirection = FVector::ZeroVector;

    bool IsWalkTargetReached = false;
    float LastDistanceToWalkTarget = 0.f;

    float MaxSplineOffset = 0.f;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    void SetSegmentationIndex(FString Name);

    void SetGuidingSpline(USplineComponent* GuidingSpline);
    void SetMaxSplineOffset(float MaxSplineOffset);
    void CheckReachedWalkTarget();

    void ResetLocation(FVector LocationOffset);
    void ResetWalkTarget(FVector LocationOffset);

    void Move(float DeltaTime);

};
