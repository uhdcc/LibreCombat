// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "WeaponStructs.h"
#include "WeaponFunctionLibrary.generated.h"

UCLASS()
class LIBRECOMBAT_API UWeaponFunctionLibrary : public UBlueprintFunctionLibrary {
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	static AWeapon* GiveWeapon(TSubclassOf<AWeapon> WeaponClass, AActor* WeaponOwner);
	UFUNCTION(BlueprintCallable)
	static void TakeWeapon(AWeapon* Weapon);
	UFUNCTION(BlueprintCallable)
	static void FinishWeaponSequence(const FWeaponSequence& WeaponSequence);

	UFUNCTION(BlueprintCallable)
	static FHitResult Hitscan(const FHitscanParameters& HitParameters);
	UFUNCTION(BlueprintCallable)
	static FHitResult HitscanInternal(const FHitscanParameters& HitParameters, const FVector& Start, const FVector& End, float Radius = 0.f);
	UFUNCTION(BlueprintCallable)
	static void HandleHitscan(const FHitResult& Hit, const FHitscanParameters& HitParameters);
	UFUNCTION(BlueprintCallable)
	static void HitscanWeaponSequence(const FHitscanParameters& HitParameters, const FWeaponSequence& Sequence);
	UFUNCTION(BlueprintCallable)
	static FHitResult CurvedHitscan(const FCurvedHitscanParameters& HitParameters);
	UFUNCTION(BlueprintCallable)
	static void CurvedHitscanWeaponSequence(const FCurvedHitscanParameters& HitParameters, const FWeaponSequence& Sequence);

	UFUNCTION(BlueprintCallable)
	static void ZoomIn(AWeapon* Weapon, float NewFov, float NewSensitivity);
	UFUNCTION(BlueprintCallable)
	static void ZoomOut(AWeapon* Weapon);

	UFUNCTION(BlueprintCallable)
	static void LaunchProjectile(const FProjectileSpawnParameters& Parameters);
	UFUNCTION(BlueprintCallable)
	static void LaunchProjectileWeaponSequence(const FProjectileSpawnParameters& Parameters, const FWeaponSequence& WeaponSequence);

	UFUNCTION(BlueprintCallable)
	static FHitResult HitscanShotgun(const FHitscanShotgunParameters& HitParameters);
	UFUNCTION(BlueprintCallable)
	static void HitscanShotgunWeaponSequence(const FHitscanShotgunParameters& HitParameters, const FWeaponSequence& Sequence);


};
