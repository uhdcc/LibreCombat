// Fill out your copyright notice in the Description page of Project Settings.


#include "Character2.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "DamageComponent.h"
#include "CharacterMovementComponent2.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "Weapon.h"
#include "Engine/AssetManager.h"

ACharacter2::ACharacter2(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCharacterMovementComponent2>(ACharacter::CharacterMovementComponentName)) {
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

	DamageComponent = CreateDefaultSubobject<UDamageComponent>(TEXT("DamageComponent"));

	isHoldingJumpButton = false;

	bCrouchButtonIsHeld = false;
	bCrouchingHack = false;
	CurrentCapsuleHeight = 0.f;
}
void ACharacter2::BeginPlay() {
	Super::BeginPlay();
	CurrentCapsuleHeight = GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
	CharacterMovement2 = Cast<UCharacterMovementComponent2>(GetCharacterMovement());
	RecalculateBaseEyeHeight();

}
void ACharacter2::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction<TDelegate<void(bool)>>("Crouch", IE_Pressed, this, &ACharacter2::Crouch2, true);
	PlayerInputComponent->BindAction<TDelegate<void(bool)>>("Crouch", IE_Released, this, &ACharacter2::Crouch2, false);
	PlayerInputComponent->BindAction<TDelegate<void(bool)>>("NextWeapon", IE_Pressed, this, &ACharacter2::CycleWeapon, true);
	PlayerInputComponent->BindAction<TDelegate<void(bool)>>("PreviousWeapon", IE_Pressed, this, &ACharacter2::CycleWeapon, false);
	PlayerInputComponent->BindAction("ThrowGrenade", IE_Pressed, this, &ACharacter2::ThrowGrenade);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if WITH_EDITOR
	UAssetManager& AssetManager = UAssetManager::Get();
	AssetManager.LoadPrimaryAssetsWithType("Weapon", TArray<FName>(), FStreamableDelegate::CreateUObject(this, &ACharacter2::AllWeaponsLoaded));
#endif
}
void ACharacter2::Tick(float DeltaTime) {
	if (bCrouchingHack) {
		CurrentCapsuleHeight += (DeltaTime * 400.f) * (bCrouchButtonIsHeld ? -1.f : 1.f);
		if (CurrentCapsuleHeight < GetCharacterMovement()->CrouchedHalfHeight) {
			CurrentCapsuleHeight = GetCharacterMovement()->CrouchedHalfHeight;
			bCrouchingHack = false;
		}
		else if (CurrentCapsuleHeight > 88.f) {
			CurrentCapsuleHeight = 88.f;
			CharacterMovement2->bCrouchHack2 = false;
			bCrouchingHack = false;
		}
		GetCapsuleComponent()->SetCapsuleHalfHeight(CurrentCapsuleHeight);
		RecalculateBaseEyeHeight();
		GetCharacterMovement()->UpdateFloorFromAdjustment();
	}
	Super::Tick(DeltaTime);
}
void ACharacter2::RecalculateBaseEyeHeight() {
	BaseEyeHeight = CurrentCapsuleHeight - 18.f;
}
void ACharacter2::Crouch2(bool bButtonWasPressed) {
	bCrouchButtonIsHeld = bButtonWasPressed;
	if (bCrouchButtonIsHeld) {
		CharacterMovement2->bCrouchHack2 = true;
	}
	bCrouchingHack = true;
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

void ACharacter2::ThrowGrenade()
{
	if (auto World = GetWorld()) {
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.Owner = this;
		SpawnParameters.Instigator = this;
		
		FTransform ThrowTransform(
			GetViewRotation(),
			GetPawnViewLocation(),
			GetActorScale3D()
		);

		World->SpawnActor<AActor>(GrenadeClass, ThrowTransform, SpawnParameters);
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
