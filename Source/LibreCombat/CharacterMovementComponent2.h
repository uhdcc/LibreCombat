// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CharacterMovementComponent2.generated.h"

USTRUCT(BlueprintType)
struct FCharacterMovementProperties {
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIgnoreBaseRotation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCanCrouch;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bAlwaysCheckFloor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bMaintainHorizontalGroundVelocity;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxWalkSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxWalkSpeedCrouched;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxAcceleration;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BrakingDecelerationWalking;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BrakingFrictionFactor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float GroundFriction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float JumpZVelocity;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BrakingDecelerationFalling;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AirControl;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AirControlBoostMultiplier;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AirControlBoostVelocityThreshold;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FallingLateralFriction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxStepHeight;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CrouchedHalfHeight;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Mass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float GravityScale;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CrouchSpeed;

	FCharacterMovementProperties() {
		bIgnoreBaseRotation = true;
		bCanCrouch = true;
		bAlwaysCheckFloor = false;
		bMaintainHorizontalGroundVelocity = false;
		MaxWalkSpeed = 600.f;
		MaxWalkSpeedCrouched = 200.f;
		MaxAcceleration = 2000.f;
		BrakingDecelerationWalking = 2200.f;
		BrakingFrictionFactor = 1.7f;
		GroundFriction = 1.f;
		JumpZVelocity = 620.f;
		BrakingDecelerationFalling = 20.f;
		AirControl = 0.09f;
		AirControlBoostMultiplier = 1.1f;
		AirControlBoostVelocityThreshold = 400.f;
		FallingLateralFriction = 0.4f;
		MaxStepHeight = 35.f;
		CrouchedHalfHeight = 60.f;
		Mass = 500.f;
		GravityScale = 1.f;
		CrouchSpeed = 300.f;
	}
};
UCLASS()
class LIBRECOMBAT_API UCharacterMovementComponent2 : public UCharacterMovementComponent
{
	GENERATED_BODY()
protected:
	virtual void BeginPlay() override;
	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;
public:
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	UCharacterMovementComponent2();
	virtual float GetMaxSpeed() const override;
	bool bCrouchIsTicking;
	float InitialCapsuleHeight;
	float CurrentCapsuleHeight;
	void Crouch2(bool bButtonWasPressed);
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCrouchButtonIsHeld;
	void Jump2(bool bButtonWasPressed);
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bJumpButtonIsHeld;


	UFUNCTION(BlueprintCallable)
	void SetCharacterMovementProperties(const FCharacterMovementProperties& NewProps);

	UPROPERTY(Category = "Character Movement (General Settings)", EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0", UIMin = "0"))
	float CrouchSpeed;
};
