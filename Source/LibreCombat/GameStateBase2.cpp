// Fill out your copyright notice in the Description page of Project Settings.


#include "GameStateBase2.h"
#include "Engine/AssetManager.h"
#include "WeaponFunctionLibrary.h"
#include "GameFramework/PlayerState.h"

#include "EngineUtils.h"
#include "Pickup.h"

void AGameStateBase2::HandleBeginPlay() {
	Super::HandleBeginPlay();

	if (auto World = GetWorld()) {
		TArray<FSoftObjectPath> SoftWeaponClasses;
		for (TActorIterator<AWeaponPickup> i(World);i; ++i) {
			SoftWeaponClasses.Add(i->WeaponClass.ToSoftObjectPath());
		}
		UAssetManager::GetStreamableManager().RequestAsyncLoad(
			SoftWeaponClasses,
			FStreamableDelegate::CreateUObject(this, &AGameStateBase2::AllPickupsLoaded)
		);
	}

	UAssetManager& AssetManager = UAssetManager::Get();
	AssetManager.LoadPrimaryAssetsWithType(
		"Weapon", 
		TArray<FName>(), 
		FStreamableDelegate::CreateUObject(this, &AGameStateBase2::AllWeaponsLoaded)
	);
}
void AGameStateBase2::OnRep_ReplicatedHasBegunPlay() {
	Super::OnRep_ReplicatedHasBegunPlay();
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if WITH_EDITOR
void AGameStateBase2::AllWeaponsLoaded() {
	UAssetManager& AssetManager = UAssetManager::Get();
	TArray<UObject*> ClassObjects;
	AssetManager.GetPrimaryAssetObjectList("Weapon", ClassObjects);
	TArray<UClass*> WeaponClasses;
	for (auto i : ClassObjects) {
		WeaponClasses.Add(Cast<UClass>(i));
	}
	for (auto Player : PlayerArray) {
		for (auto WeaponClass : WeaponClasses) {
			UWeaponFunctionLibrary::GiveWeapon(WeaponClass, Player->GetPawn());
		}
	}
}
void AGameStateBase2::AllPickupsLoaded() {
}
#endif
