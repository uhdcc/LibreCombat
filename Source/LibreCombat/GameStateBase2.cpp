// Fill out your copyright notice in the Description page of Project Settings.


#include "GameStateBase2.h"
#include "EngineUtils.h"
#include "Character2.h"
#include "Weapon.h"
#include "Engine/AssetManager.h"

void AGameStateBase2::HandleBeginPlay() {
	Super::HandleBeginPlay();

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if WITH_EDITOR
	UAssetManager& AssetManager = UAssetManager::Get();
	AssetManager.LoadPrimaryAssetsWithType(
		"Weapon", 
		TArray<FName>(), 
		FStreamableDelegate::CreateUObject(this, &AGameStateBase2::AllWeaponsLoaded)
	);
#endif

}

void AGameStateBase2::OnRep_ReplicatedHasBegunPlay() {
	Super::OnRep_ReplicatedHasBegunPlay();
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if WITH_EDITOR

void AGameStateBase2::AllWeaponsLoaded() {
	UAssetManager& AssetManager = UAssetManager::Get();
	TArray<UObject*> WeaponData;
	AssetManager.GetPrimaryAssetObjectList("Weapon", WeaponData);
	FActorSpawnParameters WeaponSpawnParams;
	if (auto World = GetWorld()) {
		for (FActorIterator It(World); It; ++It) {
			if (auto Character2 = Cast<ACharacter2>(*It)){
				WeaponSpawnParams.Owner = Character2;
				for (auto& i : WeaponData) {
					auto WeaponClass = Cast<UClass>(i);
					auto WeaponObject = GetWorld()->SpawnActor<AWeapon>(WeaponClass, WeaponSpawnParams);
					Character2->Weapons.Add(WeaponObject);
				}
				Character2->SelectWeapon(0);
			}
		}
	}
}
#endif

