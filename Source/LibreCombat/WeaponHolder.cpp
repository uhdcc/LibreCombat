// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponHolder.h"
#include "Weapon.h"
#include "WeaponFunctionLibrary.h"
#include "Pickup.h"
#include "EngineUtils.h"
#include "Engine/AssetManager.h"

UWeaponHolder::UWeaponHolder() {
	bInteractButtonIsHeld = false;
	bWeaponSwitchingHasBeenBinded = false;
}
void UWeaponHolder::BeginPlay() {
	Super::BeginPlay();
	if (GetOwner()) {	
		GrenadeSpawnParams.ProjectileOwner = GetOwner();
		if (GetOwner()->InputComponent) {
			if (GrenadeSpawnParams.ProjectileClass) {
				GetOwner()->InputComponent->BindAction("ThrowGrenade", IE_Pressed, this, &UWeaponHolder::ThrowGrenade);
			}
			if (Weapons.IsValidIndex(0)) {
				GetOwner()->InputComponent->BindAction<TDelegate<void(bool)>>("NextWeapon", IE_Pressed, this, &UWeaponHolder::CycleWeapon, true);
				GetOwner()->InputComponent->BindAction<TDelegate<void(bool)>>("PreviousWeapon", IE_Pressed, this, &UWeaponHolder::CycleWeapon, false);
				bWeaponSwitchingHasBeenBinded = true;
			}
			GetOwner()->InputComponent->BindAction<TDelegate<void(bool)>>("Interact", IE_Pressed, this, &UWeaponHolder::Interact, true);
			GetOwner()->InputComponent->BindAction<TDelegate<void(bool)>>("Interact", IE_Released, this, &UWeaponHolder::Interact, false);
			GetOwner()->OnActorBeginOverlap.AddDynamic(this, &UWeaponHolder::OnOverlap);
			GetOwner()->OnActorEndOverlap.AddDynamic(this, &UWeaponHolder::OnEndOverlap);
		}
	}	
}
void UWeaponHolder::Interact(bool bButtonWasPressed) {
	bInteractButtonIsHeld = bButtonWasPressed;
	if (bInteractButtonIsHeld) {
		InteractEvent();
	}
}
void UWeaponHolder::OnOverlap(AActor* OverlappedActor, AActor* OtherActor) {
	if (auto Pickup = Cast<APickup>(OtherActor)) {
		if (Pickup->bNeedToHoldButtonToPickUp && !bInteractButtonIsHeld) {
			return;
		}
		InteractEvent();
	}
}
void UWeaponHolder::OnEndOverlap(AActor* OverlappedActor, AActor* OtherActor) {
}
void UWeaponHolder::InteractEvent() {
	TArray<AActor*> OverlappedActors;
	GetOwner()->GetOverlappingActors(OverlappedActors, APickup::StaticClass());

	if (OverlappedActors.IsValidIndex(0)) {
		bInteractButtonIsHeld = false;
		TArray<APickup*> OverlappedPickups;
		for (auto i : OverlappedActors) {
			if (Cast<APickup>(i)) {
				OverlappedPickups.Add(Cast<APickup>(i));
			}
		}
		FVector OwnerLocation = GetOwner()->GetActorLocation();
		float TestingDistance = 0.f;
		float ClosestDistance = 0.f;
		APickup* ClosestPickup = nullptr;

		for (auto i : OverlappedPickups) {
			TestingDistance = FVector::Distance(OwnerLocation, i->GetActorLocation());
			if (TestingDistance < ClosestDistance) {
				ClosestDistance = TestingDistance;
				ClosestPickup = i;
			}
			else if (ClosestDistance == 0) {
				ClosestDistance = TestingDistance;
				ClosestPickup = i;
			}
		}
		if (ClosestPickup) {
			ClosestPickup->ApplyPickupEffect(GetOwner());
		}
	}
}
void UWeaponHolder::CycleWeapon(bool bDirection) {
	int NewWeaponIndex;
	if (!bDirection) {
		NewWeaponIndex = (CurrentWeaponIndex + 1) % Weapons.Num();
	}
	else {
		NewWeaponIndex = (CurrentWeaponIndex < 1) ?
			Weapons.Num() - 1 :
			CurrentWeaponIndex - 1;
	}
	SelectWeapon(NewWeaponIndex);
}
void UWeaponHolder::SelectWeapon(int WeaponIndex) {
	if (Weapons.IsValidIndex(WeaponIndex)) {
		Weapons[CurrentWeaponIndex]->Unequip();
		CurrentWeaponIndex = WeaponIndex;
		Weapons[CurrentWeaponIndex]->Equip();
	}
}
void UWeaponHolder::ThrowGrenade() {
	UWeaponFunctionLibrary::LaunchProjectile(GrenadeSpawnParams);
}
bool UWeaponHolder::ReplaceCurrentWeapon(TSubclassOf<AWeapon> NewWeaponClass) {
	if (!NewWeaponClass) {
		return false;
	}
	if (auto World = GetWorld()) {
		for (auto i : Weapons) {
			auto CurrentWeaponClassName = i->GetClass()->GetName();
			if (NewWeaponClass->GetName() == CurrentWeaponClassName) {
				return false;
			}
		}
		if (Weapons.Num() > 1) {
			UAssetManager& AssetManager = UAssetManager::Get();
			TArray<UObject*> AssetObjects;
			AssetManager.GetPrimaryAssetObjectList("Pickup", AssetObjects);
			auto CurrentWeapon = Weapons[CurrentWeaponIndex];
			auto CurrentWeaponClassName = CurrentWeapon->GetClass()->GetName();
			for (auto& i : AssetObjects) {
				auto AssetClass = Cast<UClass>(i);
				auto AssetObject = Cast<AWeaponPickup>(AssetClass->GetDefaultObject());
				if (AssetObject) {
					if (AssetObject->WeaponClass->GetName() == CurrentWeaponClassName) {
						DropPickup(AssetClass, GetOwner());
					}
				}
				UWeaponFunctionLibrary::TakeWeapon(CurrentWeapon);
				CurrentWeapon->Destroy();
			}
		}
		UWeaponFunctionLibrary::GiveWeapon(NewWeaponClass, GetOwner());
		return true;
	}
	return false;
}
void UWeaponHolder::DropPickup(TSubclassOf<APickup> PickupClass, AActor* PickupOwner) {
	if (PickupClass && PickupOwner) {
		auto DroppedPickup = GetWorld()->SpawnActorDeferred<APickup>(
			PickupClass,
			FTransform(),
			PickupOwner,
			PickupOwner->GetInstigator(),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn
		);
		DroppedPickup->bWasDropped = true;
		DroppedPickup->FinishSpawning(
			FTransform(
				PickupOwner->GetActorRotation(),
				PickupOwner->GetActorLocation() + PickupOwner->GetActorForwardVector() * 200.f,
				PickupOwner->GetActorScale3D()
			)
		);
	}
}

int UWeaponHolder::AddWeapon(AWeapon* NewWeapon) {
	if (!bWeaponSwitchingHasBeenBinded && GetOwner()->InputComponent) {
		GetOwner()->InputComponent->BindAction<TDelegate<void(bool)>>("NextWeapon", IE_Pressed, this, &UWeaponHolder::CycleWeapon, true);
		GetOwner()->InputComponent->BindAction<TDelegate<void(bool)>>("PreviousWeapon", IE_Pressed, this, &UWeaponHolder::CycleWeapon, false);
		bWeaponSwitchingHasBeenBinded = true;
	}
	return Weapons.Add(NewWeapon);
}
