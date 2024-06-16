// Fill out your copyright notice in the Description page of Project Settings.


#include "Character2.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Weapon.h"
#include "Engine/AssetManager.h"

ACharacter2::ACharacter2()
{
	GetCharacterMovement()->bIgnoreBaseRotation = true;
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	GetCharacterMovement()->bAlwaysCheckFloor = false;
	GetCharacterMovement()->bMaintainHorizontalGroundVelocity = false;
	GetCharacterMovement()->MaxWalkSpeed = 600.f;
	GetCharacterMovement()->MaxWalkSpeedCrouched = 200.f;
	GetCharacterMovement()->MaxAcceleration = 2000.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2200.f;
	GetCharacterMovement()->BrakingFrictionFactor = 1.7f;
	GetCharacterMovement()->GroundFriction = 1.f;
	GetCharacterMovement()->JumpZVelocity = 620.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 20.f;
	GetCharacterMovement()->AirControl = 0.09f;
	GetCharacterMovement()->AirControlBoostMultiplier = 1.1f;
	GetCharacterMovement()->AirControlBoostVelocityThreshold = 400.f;
	GetCharacterMovement()->FallingLateralFriction = 0.4f;
	GetCharacterMovement()->MaxStepHeight = 35.f;

	GetCapsuleComponent()->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_Yes;
	GetMesh()->bOwnerNoSee = true;

	isHoldingJumpButton = false;
}

void ACharacter2::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction<TDelegate<void(bool)>>("Crouch", IE_Pressed, this, &ACharacter::Crouch, false);
	PlayerInputComponent->BindAction<TDelegate<void(bool)>>("Crouch", IE_Released, this, &ACharacter::UnCrouch, false);
	PlayerInputComponent->BindAction<TDelegate<void(bool)>>("NextWeapon", IE_Pressed, this, &ACharacter2::CycleWeapon, true);
	PlayerInputComponent->BindAction<TDelegate<void(bool)>>("PreviousWeapon", IE_Released, this, &ACharacter2::CycleWeapon, false);

#if WITH_EDITOR
	UAssetManager& AssetManager = UAssetManager::Get();
	AssetManager.LoadPrimaryAssetsWithType("Weapon", TArray<FName>(), FStreamableDelegate::CreateUObject(this, &ACharacter2::AllWeaponsLoaded));
#endif
}

#if WITH_EDITOR
void ACharacter2::AllWeaponsLoaded() {
	UAssetManager& AssetManager = UAssetManager::Get();
	TArray<UObject*> WeaponData;
	AssetManager.GetPrimaryAssetObjectList("Weapon", WeaponData);
	FActorSpawnParameters WeaponSpawnParams;
	WeaponSpawnParams.Owner = this;
	for(auto& i : WeaponData) {
		auto WeaponClass = Cast<UClass>(i);
		auto WeaponObject = GetWorld()->SpawnActor<AWeapon>(WeaponClass, WeaponSpawnParams);
		Weapons.Add(WeaponObject);
	}
	SelectWeapon(0);

}
#endif

void ACharacter2::CycleWeapon(bool bDirection) {
		int NewWeaponIndex;
		if(!bDirection) {
			NewWeaponIndex = (CurrentWeaponIndex + 1) % Weapons.Num();
		}
		else {
			NewWeaponIndex = (CurrentWeaponIndex < 1) ?
				Weapons.Num() - 1 :
				CurrentWeaponIndex - 1;
		}
		SelectWeapon(NewWeaponIndex);
}

void ACharacter2::SelectWeapon(int WeaponIndex) {
	if(Weapons.IsValidIndex(WeaponIndex)) {
		Weapons[CurrentWeaponIndex]->Unequip();
		CurrentWeaponIndex = WeaponIndex;
		Weapons[CurrentWeaponIndex]->Equip();
	}
}

void ACharacter2::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode) {
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);
	if(isHoldingJumpButton) {
		if(GetCharacterMovement()->MovementMode == MOVE_Walking) {
			Jump();
		}
	}
}
void ACharacter2::Jump() {
	isHoldingJumpButton = true;
	Super::Jump();
}
void ACharacter2::StopJumping() {
	isHoldingJumpButton = false;
	Super::StopJumping();
}
