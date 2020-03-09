// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MultiplayerShooterCharacter.h"
#include "Animation/AnimInstance.h"
#include "BaseAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERSHOOTER_API UBaseAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
/** Properties **/
public:
	//
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	AMultiplayerShooterCharacter* OwningChar;
	//
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	float MoveFwdBwd;
	//
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	float Direction;
	//
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	float AxisTurn;
	//
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	EMovementStates CurrentCharMovStates;
	//
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	bool isMoving;

/** Functions **/
public:
	// Constructor
	UBaseAnimInstance();
	//
	virtual void NativeInitializeAnimation() override;
	//
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

};
