// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "WeaponPickupMaster.h"
#include "Components/ActorComponent.h"
#include "MultiplayerShooterCharacter.generated.h"

UENUM(BlueprintType)
enum class EMovementStates : uint8
{
	WALK UMETA(DisplayName = "Walk"),
	JOG UMETA(DisplayName = "Jog"),
	SPRINT UMETA(DisplayName = "Sprint")
};

UCLASS(config=Game)
class AMultiplayerShooterCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
public:
	AMultiplayerShooterCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

protected:

	/** Called every frame **/
	virtual void Tick(float DeltaSeconds) override;

	/** Set post initialize components **/
	void PostInitializeComponents() override;

	/** Set net game mode **/
	bool IsNetworked();

	/** Resets HMD orientation in VR. */
	void OnResetVR();

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/** Called for interact with objects */
	void Interact();

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	void Turn(float Value);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

	/** Network and Animation replication **/
	UFUNCTION(Reliable, Server)
	void MoveForwardOnServer(float value);

	UFUNCTION(Reliable, Server)
	void MoveRightOnServer(float value);

	UFUNCTION(Reliable, Server)
	void AxisTurnOnServer(float value);

	UFUNCTION(Reliable, Server)
	void ServerDestroyPickup(AActor* PickupToBeDestroyed);

	UFUNCTION(Reliable, Server)
	void ServerSpawnWeapon(TSubclassOf<AWeaponActualMaster> NewWeapon);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	/** Animation properties **/
public:
	//
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Animation")
	float MoveLeftRight;
	//
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Animation")
	float MoveFwdBwd;
	//
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Animation")
	float AxisTurn;
	//
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Animation")
	EMovementStates CharacterMovStates;
	//
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Animation")
	bool isMoving;

	/** Weapon properties **/
public:
	//
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	TArray<TSubclassOf<AWeaponActualMaster>> WeaponInventory;
	//
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	AWeaponActualMaster* CurrentWeapon;
	//
	int32 LastIndex = -1;

};

