// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterMovementComponent2.h"
#include "GameFramework/Character.h"

UCharacterMovementComponent2::UCharacterMovementComponent2() {
	bUseCrouchWalkingSpeed = false;
}
float UCharacterMovementComponent2::GetMaxSpeed() const {
	switch (MovementMode) {
	case MOVE_Walking:
	case MOVE_NavWalking:
		return bUseCrouchWalkingSpeed ? MaxWalkSpeedCrouched : MaxWalkSpeed;
	case MOVE_Falling:
		return MaxWalkSpeed;
	case MOVE_Swimming:
		return MaxSwimSpeed;
	case MOVE_Flying:
		return MaxFlySpeed;
	case MOVE_Custom:
		return MaxCustomMovementSpeed;
	case MOVE_None:
	default:
		return 0.f;
	}
}
