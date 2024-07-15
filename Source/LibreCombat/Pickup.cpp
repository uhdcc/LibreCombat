// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickup.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"

#include "Weapon.h"
#include "WeaponHolder.h"
#include "WeaponFunctionLibrary.h"

APickup::APickup() {
	PickupCollision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("PickupCollision"));
	PickupCollision->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	PickupCollision->InitCapsuleSize(10.f, 30.f);
	PickupCollision->SetRelativeRotation(FRotator(0.f, 0.f, 270.f));
	RootComponent = PickupCollision;

	PickupTrigger = CreateDefaultSubobject<USphereComponent>(TEXT("PickupTrigger"));
	PickupTrigger->SetSphereRadius(200.f);
	PickupTrigger->SetupAttachment(RootComponent);

	PickupModel = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("PickupModel"));
	PickupModel->SetCollisionProfileName(TEXT("NoCollision"));
	PickupModel->SetupAttachment(RootComponent);
	PickupModel->SetRelativeScale3D(FVector(2.f));

	PickupMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("PickupMovement"));
	PickupMovement->bSimulationEnabled = false;
	PickupMovement->bShouldBounce = true;
	PickupMovement->Bounciness = 0.4f;
	PickupMovement->Friction = 0.4f;

	TimerMode = EPickupTimerMode::WhenMatchStarts;
	bNeedToHoldButtonToPickUp = true;
	RespawnTime = 30.f;
}
void APickup::BeginPlay() {
	Super::BeginPlay();
	if (auto OwningActor = GetOwner()) {
		PickupCollision->IgnoreActorWhenMoving(OwningActor, true);
		if (bWasDropped) {
			PickupMovement->Velocity = (GetActorForwardVector() * 200.f) + (OwningActor->GetVelocity() * 1.1f);
		}
	}
	if (bWasDropped) {
		PickupMovement->bSimulationEnabled = true;
		PickupMovement->SetUpdatedComponent(PickupCollision);
	}
}
void APickup::Respawn() {
	SetActorEnableCollision(true);
	SetActorHiddenInGame(false);
}

void AWeaponPickup::OnConstruction(const FTransform& Transform) {
	Super::OnConstruction(Transform);
	if (!PickupModel->SkeletalMesh && WeaponClass) {
		auto TempWeapon = GetWorld()->SpawnActor<AWeapon>(WeaponClass.Get());
		PickupModel->SetSkeletalMesh(TempWeapon->FirstPersonMesh->SkeletalMesh);
		TempWeapon->Destroy();
	}
}

void AWeaponPickup::ApplyPickupEffect_Implementation(AActor* PickerUpper) {
	if (auto WeaponHolder = PickerUpper->FindComponentByClass<UWeaponHolder>()) {
		if (!WeaponHolder->ReplaceCurrentWeapon(WeaponClass.Get())) {
			//if (true) { // has room for ammo?
			//	// take ammo
			//}
		}
		else {
			if (auto World = GetWorld()) {
				SetActorEnableCollision(false);
				SetActorHiddenInGame(true);
				if (!bWasDropped) {
					World->GetTimerManager().SetTimer(
						*new FTimerHandle(),
						this, &APickup::Respawn,
						5.f,
						false
					);
				}
				else {
					Destroy();
				}
			}
		}
	}
}
