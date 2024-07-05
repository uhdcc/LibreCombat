// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Character2.generated.h"

class AWeapon;
class UDamageComponent;
class UCharacterMovementComponent2;

UCLASS()
class LIBRECOMBAT_API ACharacter2 : public ACharacter
{
	GENERATED_BODY()
public:
	ACharacter2(const FObjectInitializer& ObjectInitializer);
	virtual void BeginPlay() override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void Tick(float DeltaTime) override;
	virtual void RecalculateBaseEyeHeight() override;
	UCharacterMovementComponent2* CharacterMovement2;

	virtual void Crouch2(bool bButtonWasPressed);
	bool bCrouchButtonIsHeld;
	bool bCrouchingHack;
	float CurrentCapsuleHeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UDamageComponent* DamageComponent;


#if WITH_EDITOR
	void AllWeaponsLoaded();
#endif
	TArray<AWeapon*> Weapons;
	int CurrentWeaponIndex;
	void CycleWeapon(bool bDirection);
	void SelectWeapon(int WeaponIndex);

	void ThrowGrenade();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UClass* GrenadeClass;



	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode = 0) override;

	virtual void Jump() override;
	virtual void StopJumping() override;
	bool isHoldingJumpButton;
};
