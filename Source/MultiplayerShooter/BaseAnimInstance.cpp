// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseAnimInstance.h"
#include "Engine/Engine.h"

UBaseAnimInstance::UBaseAnimInstance()
{
	MoveFwdBwd = 0.0f;
	Direction = 0.0f;
	isMoving = false;

	CurrentCharMovStates = EMovementStates::JOG;
}

void UBaseAnimInstance::NativeInitializeAnimation()
{
	OwningChar = Cast<AMultiplayerShooterCharacter>(GetOwningActor());
}

void UBaseAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	if (OwningChar)
	{
		MoveFwdBwd = OwningChar->MoveFwdBwd;
		Direction = OwningChar->MoveLeftRight;
		CurrentCharMovStates = OwningChar->CharacterMovStates;
		isMoving = OwningChar->isMoving;
		AxisTurn = OwningChar->AxisTurn;
	}
}
