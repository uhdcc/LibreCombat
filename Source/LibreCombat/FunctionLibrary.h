// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FunctionLibrary.generated.h"

class AWeapon;
class UNiagaraSystem;

USTRUCT(BlueprintType)
struct FHitscanSequenceParameters {
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimationAsset* Animation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UNiagaraSystem* BulletTrail;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* WeaponSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* HitSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInterface* BulletDecal;

	FHitscanSequenceParameters() {
		Animation = nullptr;
		BulletTrail = nullptr;
		WeaponSound = nullptr;
		HitSound = nullptr;
		BulletDecal = nullptr;
	}
};

USTRUCT(BlueprintType)
struct FHitscanParameters {
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AWeapon* Weapon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PhysicsForce;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Damage;

	FHitscanParameters() {
		Weapon = nullptr;
		PhysicsForce = 60000.f;
		Damage = 34.f;
	}
};

UCLASS()
class LIBRECOMBAT_API UFunctionLibrary : public UBlueprintFunctionLibrary {
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	static FHitResult Hitscan(const FHitscanParameters& HitParameters);
	UFUNCTION(BlueprintCallable)
	static void HitscanWeaponSequence(const FHitscanParameters& HitParameters, const FHitscanSequenceParameters& Sequence);
	UFUNCTION(BlueprintCallable, meta = (DefaultToSelf = "Weapon"))
	static void ZoomIn(AWeapon* Weapon, float NewFov, float NewSensitivity);
	UFUNCTION(BlueprintCallable, meta = (DefaultToSelf = "Weapon"))
	static void ZoomOut(AWeapon* Weapon);
};
