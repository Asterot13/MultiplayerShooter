// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "MultiplayerShooterCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/SpringArmComponent.h"
#include "Engine/Engine.h"

//////////////////////////////////////////////////////////////////////////
// AMultiplayerShooterCharacter

AMultiplayerShooterCharacter::AMultiplayerShooterCharacter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	PrimaryActorTick.bCanEverTick = true;

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

	CharacterMovStates = EMovementStates::JOG;
	MoveLeftRight = 0.0f;
	MoveFwdBwd = 0.0f;

	// Replication
	bReplicates = true;
	bReplicateMovement = true;
	bNetUseOwnerRelevancy = true;

	GetMovementComponent()->SetIsReplicated(true);
	GetCharacterMovement()->SetIsReplicated(true);

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}

//////////////////////////////////////////////////////////////////////////
// Input

void AMultiplayerShooterCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AMultiplayerShooterCharacter::Interact);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMultiplayerShooterCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMultiplayerShooterCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &AMultiplayerShooterCharacter::Turn);
	PlayerInputComponent->BindAxis("TurnRate", this, &AMultiplayerShooterCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AMultiplayerShooterCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &AMultiplayerShooterCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &AMultiplayerShooterCharacter::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AMultiplayerShooterCharacter::OnResetVR);
}

void AMultiplayerShooterCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(AMultiplayerShooterCharacter, MoveLeftRight);
	DOREPLIFETIME(AMultiplayerShooterCharacter, MoveFwdBwd);
	DOREPLIFETIME(AMultiplayerShooterCharacter, CharacterMovStates);
	DOREPLIFETIME(AMultiplayerShooterCharacter, isMoving);
	DOREPLIFETIME(AMultiplayerShooterCharacter, AxisTurn);
}

void AMultiplayerShooterCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	isMoving = (MoveFwdBwd != 0.0f || MoveLeftRight != 0.0f) ? true : false;
}

void AMultiplayerShooterCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

bool AMultiplayerShooterCharacter::IsNetworked()
{
	if (GetWorld()->GetNetMode() == NM_Standalone)
	{
		return false;
	}

	return true;
}

void AMultiplayerShooterCharacter::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void AMultiplayerShooterCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
		Jump();
}

void AMultiplayerShooterCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
		StopJumping();
}

void AMultiplayerShooterCharacter::MoveForwardOnServer_Implementation(float value)
{
	MoveFwdBwd = value;
}

void AMultiplayerShooterCharacter::MoveRightOnServer_Implementation(float value)
{
	MoveLeftRight = value;
}

void AMultiplayerShooterCharacter::AxisTurnOnServer_Implementation(float value)
{
	AxisTurn = value;
}

void AMultiplayerShooterCharacter::ServerDestroyPickup_Implementation(AActor* PickupToBeDestroyed)
{
	PickupToBeDestroyed->Destroy();
}

void AMultiplayerShooterCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AMultiplayerShooterCharacter::Turn(float Value)
{
	if (AxisTurn != Value)
	{
		AxisTurnOnServer(Value);
	}
	AddControllerYawInput(Value);
}

void AMultiplayerShooterCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AMultiplayerShooterCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		if (MoveFwdBwd != Value)
		{
			MoveForwardOnServer(Value);
		}

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
	else
	{
		if (MoveFwdBwd != Value)
		{
			MoveForwardOnServer(Value);
		}
	}
}

void AMultiplayerShooterCharacter::MoveRight(float Value)
{
	if ( (Controller != NULL) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		if (MoveLeftRight != Value)
		{
			MoveRightOnServer(Value);
		}

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
	else 
	{
		if (MoveLeftRight != Value)
		{
			MoveRightOnServer(Value);
		}
	}
}

void AMultiplayerShooterCharacter::Interact()
{
	TArray<AActor*> OverlapedActors;
	GetOverlappingActors(OverlapedActors, TSubclassOf<AWeaponPickupMaster>());

	for (AActor* weapon : OverlapedActors)
	{
		AWeaponPickupMaster* WeaponPickup = Cast<AWeaponPickupMaster>(weapon);
		AWeaponActualMaster* tempWeaponActual = WeaponPickup->WeaponActual.GetDefaultObject();
		int32 Index = WeaponInventory.AddUnique(tempWeaponActual);
		if (Index == -1 || Index == LastIndex)
		{			
			GEngine->AddOnScreenDebugMessage(0, 2.0f, FColor::Cyan, TEXT("Did not added!"));
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(0, 2.0f, FColor::Cyan, FString::FromInt(Index));
			LastIndex = Index;
			ServerDestroyPickup(weapon);
		}
		/*AWeaponActualMaster* temp = *WeaponInventory.GetData();
		GEngine->AddOnScreenDebugMessage(0, 2.0f, FColor::Cyan, temp->GetName());*/
	}
}
