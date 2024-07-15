// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterMovementComponent2.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"

UCharacterMovementComponent2::UCharacterMovementComponent2() {
	bCrouchButtonIsHeld = false;
	CrouchSpeed = 300.f;
	bJumpButtonIsHeld = false;
}
void UCharacterMovementComponent2::BeginPlay() {
	Super::BeginPlay();
	SetCharacterMovementProperties(FCharacterMovementProperties());
	if (CharacterOwner) {
		InitialCapsuleHeight = CurrentCapsuleHeight = CharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
		CharacterOwner->RecalculateBaseEyeHeight();
		if (CharacterOwner->InputComponent) {
			CharacterOwner->InputComponent->BindAction<TDelegate<void(bool)>>("Jump", IE_Pressed, this, &UCharacterMovementComponent2::Jump2, true);
			CharacterOwner->InputComponent->BindAction<TDelegate<void(bool)>>("Jump", IE_Released, this, &UCharacterMovementComponent2::Jump2, false);
			CharacterOwner->InputComponent->BindAction<TDelegate<void(bool)>>("Crouch", IE_Pressed, this, &UCharacterMovementComponent2::Crouch2, true);
			CharacterOwner->InputComponent->BindAction<TDelegate<void(bool)>>("Crouch", IE_Released, this, &UCharacterMovementComponent2::Crouch2, false);
		}
	}
}
void UCharacterMovementComponent2::Crouch2(bool bButtonWasPressed) {
	bCrouchButtonIsHeld = bButtonWasPressed;
	bCrouchIsTicking = true;
}
void UCharacterMovementComponent2::Jump2(bool bButtonWasPressed) {
	bJumpButtonIsHeld = bButtonWasPressed;
	if (bJumpButtonIsHeld) {
		CharacterOwner->Jump();
	}
	else {
		CharacterOwner->StopJumping();

	}
}
void UCharacterMovementComponent2::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) {
	Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);
	if (bJumpButtonIsHeld) {
		if (MovementMode == MOVE_Walking) {
			Jump2(true);
		}
	}
}
void UCharacterMovementComponent2::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	if (bCrouchIsTicking) {
		CurrentCapsuleHeight += (DeltaTime * CrouchSpeed) * (bCrouchButtonIsHeld ? -1.f : 1.f);
		if (CurrentCapsuleHeight < CrouchedHalfHeight) {
			CurrentCapsuleHeight = CrouchedHalfHeight;
			bCrouchIsTicking = false;
		}
		else if (CurrentCapsuleHeight > InitialCapsuleHeight) {
			CurrentCapsuleHeight = InitialCapsuleHeight;
			bCrouchIsTicking = false;
		}
		CharacterOwner->GetCapsuleComponent()->SetCapsuleHalfHeight(CurrentCapsuleHeight);
		CharacterOwner->RecalculateBaseEyeHeight();
		UpdateFloorFromAdjustment();
	}
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}
float UCharacterMovementComponent2::GetMaxSpeed() const {
	switch (MovementMode) {
		case MOVE_Walking:
		case MOVE_NavWalking:
			return bCrouchButtonIsHeld ? MaxWalkSpeedCrouched : MaxWalkSpeed;
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
void UCharacterMovementComponent2::SetCharacterMovementProperties(const FCharacterMovementProperties& NewProps) {
	bIgnoreBaseRotation = NewProps.bIgnoreBaseRotation;
	NavAgentProps.bCanCrouch = NewProps.bCanCrouch;
	bAlwaysCheckFloor = NewProps.bAlwaysCheckFloor;
	bMaintainHorizontalGroundVelocity = NewProps.bMaintainHorizontalGroundVelocity;
	MaxWalkSpeed = NewProps.MaxWalkSpeed;
	MaxWalkSpeedCrouched = NewProps.MaxWalkSpeedCrouched;
	MaxAcceleration = NewProps.MaxAcceleration;
	BrakingDecelerationWalking = NewProps.BrakingDecelerationWalking;
	BrakingFrictionFactor = NewProps.BrakingFrictionFactor;
	GroundFriction = NewProps.GroundFriction;
	JumpZVelocity = NewProps.JumpZVelocity;
	BrakingDecelerationFalling = NewProps.BrakingDecelerationFalling;
	AirControl = NewProps.AirControl;
	AirControlBoostMultiplier = NewProps.AirControlBoostMultiplier;
	AirControlBoostVelocityThreshold = NewProps.AirControlBoostVelocityThreshold;
	FallingLateralFriction = NewProps.FallingLateralFriction;
	MaxStepHeight = NewProps.MaxStepHeight;
	CrouchedHalfHeight = NewProps.CrouchedHalfHeight;
	Mass = NewProps.Mass;
	GravityScale = NewProps.GravityScale;
	CrouchSpeed = NewProps.CrouchSpeed;
}


