// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "EndlessRunnerCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/SplineComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"

#include "Tiles/BaseTile.h"

#include "DrawDebugHelpers.h"


//////////////////////////////////////////////////////////////////////////
// AEndlessRunnerCharacter

AEndlessRunnerCharacter::AEndlessRunnerCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}

//////////////////////////////////////////////////////////////////////////
// Input

void AEndlessRunnerCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &AEndlessRunnerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AEndlessRunnerCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AEndlessRunnerCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AEndlessRunnerCharacter::LookUpAtRate);
}



void AEndlessRunnerCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AEndlessRunnerCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AEndlessRunnerCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
        AddMovementInput(Direction, Value);
	}
}

void AEndlessRunnerCharacter::MoveRight(float Value)
{
	if ( (Controller != NULL) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void AEndlessRunnerCharacter::BeginPlay(){
    Super::BeginPlay();

    GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void AEndlessRunnerCharacter::Tick(float DeltaSeconds){
    Super::Tick(DeltaSeconds);

    FVector traceStart = GetActorLocation() + (GetActorRotation().Vector() * FVector(0.f, 0.f, 0.f));
    FVector traceForward = FVector(0.f, 0.f, -1.f);
    FVector traceEnd = traceStart + traceForward*5000.f;

    DrawDebugLine(GetWorld(), traceStart, traceEnd, FColor::Green, false, 1, 0, 1);

    FHitResult outHit;
    FCollisionQueryParams CollisionParams;

    if(GetWorld()->LineTraceSingleByChannel(outHit, traceStart, traceEnd, ECC_Visibility, CollisionParams)){
            if(outHit.bBlockingHit){
                //UE_LOG(LogTemp, Log, TEXT("Hitting: %s"), *outHit.GetActor()->GetName());
                if(outHit.GetActor()->IsA(ABaseTile::StaticClass())
                        && LastGuideSplineName != outHit.GetActor()->GetName()){
                    LastGuideSplineName = outHit.GetActor()->GetName();
                    //UE_LOG(LogTemp, Log, TEXT("Hitting: %s"), *outHit.GetActor()->GetName());
                    //UE_LOG(LogTemp, Log, TEXT("usable tile, setting spline"));
                    GuideSpline = Cast<ABaseTile>(outHit.GetActor())->GetMeshSpline(
                                GetActorLocation());
                }
            }
    }
    if(GuideSpline){
        FVector direction = GuideSpline->FindRotationClosestToWorldLocation(GetActorLocation(), ESplineCoordinateSpace::World).Vector();
        //AddMovementInput(direction);
    }


    //FVector direction = FVector(1.f, 0.f, 0.f);
    //TODO: Uncomment
    //AddMovementInput(direction);
}
