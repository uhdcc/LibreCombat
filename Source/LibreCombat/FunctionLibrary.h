// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FunctionLibrary.generated.h"

class AWeapon;
class UNiagaraSystem;

USTRUCT(BlueprintType)
struct FWeaponSequence {
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AWeapon* Weapon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimationAsset* Animation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* WeaponSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* HitSound;

	FWeaponSequence() {
		Weapon = nullptr;
		Animation = nullptr;
		WeaponSound = nullptr;
		HitSound = nullptr;
	}
};
USTRUCT(BlueprintType)
struct FHitscanSequence : public FWeaponSequence {
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UNiagaraSystem* BulletTrail;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInterface* BulletDecal;

	FHitscanSequence() {
		BulletTrail = nullptr;
		BulletDecal = nullptr;
		Weapon = nullptr;
		Animation = nullptr;
		WeaponSound = nullptr;
		HitSound = nullptr;
	}
};
USTRUCT(BlueprintType)
struct FHitscanParameters {
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* Owner;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PhysicsForce;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Damage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<AActor*> IgnoredActors;

	FHitscanParameters() {
		Owner = nullptr;
		PhysicsForce = 60000.f;
		Damage = 34.f;
		IgnoredActors = TArray<AActor*>();
	}
};
USTRUCT(BlueprintType)
struct FProjectileSpawnParameters {
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* Owner;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> Class;

	FProjectileSpawnParameters() {
		Owner = nullptr;
		Class = nullptr;
	}
};
UCLASS()
class LIBRECOMBAT_API UFunctionLibrary : public UBlueprintFunctionLibrary {
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	static FHitResult CurvedHitscan(const FHitscanParameters& HitParameters);
	UFUNCTION(BlueprintCallable)
	static FHitResult Hitscan(const FHitscanParameters& HitParameters);
	UFUNCTION(BlueprintCallable)
	static void HandleHitscan(const FHitResult& Hit, const FHitscanParameters& HitParameters);
	UFUNCTION(BlueprintCallable)
	static void HitscanWeaponSequence(const FHitscanParameters& HitParameters, const FHitscanSequence& Sequence);
	UFUNCTION(BlueprintCallable)
	static void ZoomIn(AWeapon* Weapon, float NewFov, float NewSensitivity);
	UFUNCTION(BlueprintCallable)
	static void ZoomOut(AWeapon* Weapon);
	UFUNCTION(BlueprintCallable)
	static void LaunchProjectile(const FProjectileSpawnParameters& Parameters, USoundBase* HitSound = nullptr);
	UFUNCTION(BlueprintCallable)
	static void LaunchProjectileWeaponSequence(const FProjectileSpawnParameters& Parameters, const FWeaponSequence& WeaponSequence);
	UFUNCTION(BlueprintCallable)
	static void FinishWeaponSequence(const FWeaponSequence& WeaponSequence);
};
