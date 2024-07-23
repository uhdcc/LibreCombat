// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "HUD2.h"

AWeapon::AWeapon()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	ThirdPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ThirdPersonMesh"));
	ThirdPersonMesh->SetupAttachment(RootComponent);
	ThirdPersonMesh->AlwaysLoadOnClient = true;
	ThirdPersonMesh->AlwaysLoadOnServer = true;
	ThirdPersonMesh->bOwnerNoSee = true;
	ThirdPersonMesh->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPose;
	ThirdPersonMesh->bCastDynamicShadow = true;
	ThirdPersonMesh->bAffectDynamicIndirectLighting = true;
	ThirdPersonMesh->PrimaryComponentTick.TickGroup = TG_PrePhysics;
	ThirdPersonMesh->SetGenerateOverlapEvents(false);
	ThirdPersonMesh->SetCanEverAffectNavigation(false);
	ThirdPersonMesh->SetIsReplicated(true);

	//FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FirstPersonMesh"));
	//FirstPersonMesh->SetupAttachment(RootComponent);
	//FirstPersonMesh->bOnlyOwnerSee = true;
	//FirstPersonMesh->CastShadow = false;
	//FirstPersonMesh->SetIsReplicated(true);

	PrimaryFire = CreateDefaultSubobject<UButtonComponent>(TEXT("PrimaryFire"));
	SecondaryFire = CreateDefaultSubobject<UButtonComponent>(TEXT("SecondaryFire"));
	Reload = CreateDefaultSubobject<UButtonComponent>(TEXT("Reload"));
	Reload->CooldownDuration = 0.f;
	Reload->ButtonEvent.AddDynamic(this, &AWeapon::OnReloadButtonEvent);

	bIsEquipped = false;
	ReloadDuration = 0.f;
	bIsReloading = false;
	bOutOfAmmo = false;
}
void AWeapon::SubtractAmmo(float SubtractionAmount) {
	Ammo.Magazine -= SubtractionAmount;
	if (Ammo.Magazine <= 0.f) {
		if (Ammo.Bandolier <= 0.f) {
			bOutOfAmmo = true;
		}
		else {
			StartReloadingAmmo();
		}
	}
	if (HUD2) {
		HUD2->SetAmmo(Ammo.Magazine, Ammo.Bandolier);
	}
}
void AWeapon::StartReloadingAmmo() {
	if (ReloadDuration > 0.f) {
		if (!bIsReloading && (Ammo.Magazine < Ammo.MagazineCapacity)) {
			if (auto World = GetWorld()) {
				bIsReloading = true;
				World->GetTimerManager().SetTimer(
					ReloadTimer,
					this, &AWeapon::FinishReloadingAmmo,
					ReloadDuration,
					false
				);
			}
		}
	}
	else {
		FinishReloadingAmmo();
	}
}
void AWeapon::OnReloadButtonEvent_Implementation(UButtonComponent* Button) {
		StartReloadingAmmo();
}
void AWeapon::FinishReloadingAmmo() {
	float AmmoNeeded = Ammo.MagazineCapacity - Ammo.Magazine;
	if (Ammo.Bandolier > AmmoNeeded) {
		Ammo.Magazine = Ammo.MagazineCapacity;
		Ammo.Bandolier -= AmmoNeeded;
	}
	else {
		Ammo.Magazine = Ammo.Bandolier;
		Ammo.Bandolier = 0.f;
	}
	if (HUD2) {
		HUD2->SetAmmo(Ammo.Magazine, Ammo.Bandolier);
	}
	bIsReloading = false;
}
void AWeapon::Equip_Implementation() {
	ThirdPersonMesh->SetVisibility(true);
	//FirstPersonMesh->SetVisibility(true);
	bIsEquipped = true;
	for (auto i : Buttons) {
		i->bIsEquipped = true;
		if (i->bButtonIsHeld) {
			i->Action(true);
		}
	}
	if (HUD2) {
		HUD2->OnEquipWeapon(HudParameters);
	}
}
void AWeapon::Unequip_Implementation() {
	ThirdPersonMesh->SetVisibility(false);
	//FirstPersonMesh->SetVisibility(false);
	bIsEquipped = false;
	for (auto i : Buttons) {
		i->bIsEquipped = false;
	}
}
UButtonComponent::UButtonComponent() {
	CooldownDuration = 0.3f;
	CooldownTimer = FTimerHandle();
	bIsCoolingDown = false;
	bButtonIsHeld = false;
	bIsEquipped = false;
}
void UButtonComponent::BindInput(UInputComponent& InputComponent) {
	InputComponent.BindAction<TDelegate<void(bool)>>(GetFName(), IE_Pressed, this, &UButtonComponent::Action, true);
	InputComponent.BindAction<TDelegate<void(bool)>>(GetFName(), IE_Released, this, &UButtonComponent::Action, false);
}
void UButtonComponent::StartCooldown() {
	bIsCoolingDown = false;
	if (bButtonIsHeld) {
		Action(true);
	}
}
void UButtonComponent::Action(bool bButtonWasPressed) {
	bButtonIsHeld = bButtonWasPressed;
	if (bIsEquipped && !bIsCoolingDown) {
		if (auto World = GetWorld()) {
			if (ButtonEvent.IsBound()) {
				ButtonEvent.Broadcast(this);
			}
			if (CooldownDuration > 0.f) {
				bIsCoolingDown = true;
				World->GetTimerManager().SetTimer(
					CooldownTimer,
					this, &UButtonComponent::StartCooldown,
					CooldownDuration,
					false
				);
			}
		}
	}
}
FAmmo::FAmmo() {
	Magazine = 0.f;
	Bandolier = 0.f;
	InitialMagazine = 0.f;
	InitialBandolier = 0.f;
	MagazineCapacity = 0.f;
	BandolierCapacity = 0.f;
}
