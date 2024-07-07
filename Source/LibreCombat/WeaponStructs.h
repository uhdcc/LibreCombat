#pragma once
//#include "WeaponStructs.h"
#include "WeaponStructs.generated.h"

class AWeapon;
class UNiagaraSystem;

USTRUCT(BlueprintType)
struct FWeaponHudParameters {
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText WeaponName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* ReticleTexture;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D ReticleSize;

	FWeaponHudParameters() {
		WeaponName = FText();
		ReticleTexture = nullptr;
		ReticleSize = FVector2D(40.f);
	}
};

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

	FWeaponSequence() {
		Weapon = nullptr;
		Animation = nullptr;
		WeaponSound = nullptr;
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UNiagaraSystem* BulletTrail;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInterface* BulletDecal;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* HitSound;

	FHitscanParameters() {
		Owner = nullptr;
		PhysicsForce = 60000.f;
		Damage = 34.f;
		IgnoredActors = TArray<AActor*>();
		BulletTrail = nullptr;
		BulletDecal = nullptr;
		HitSound = nullptr;
	}
};
USTRUCT(BlueprintType)
struct FCurvedHitscanParameters : public FHitscanParameters {
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Angle;

	FCurvedHitscanParameters() {
		Angle = 0.9999f;
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* HitSound;

	FProjectileSpawnParameters() {
		Owner = nullptr;
		Class = nullptr;
		HitSound = nullptr;

	}
};
USTRUCT(BlueprintType)
struct FShotgunPatternParameters {
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Angle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int NumberOfPellets;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int NumberOfRings;

	FShotgunPatternParameters() {
		Angle = 3.f;
		NumberOfPellets = 16;
		NumberOfRings = 2;
	}
};
USTRUCT(BlueprintType)
struct FHitscanShotgunParameters : public FHitscanParameters {
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FShotgunPatternParameters Pattern;

	FHitscanShotgunParameters() {
		Pattern = FShotgunPatternParameters();
		Owner = nullptr;
		PhysicsForce = 60000.f;
		Damage = 34.f;
		IgnoredActors = TArray<AActor*>();
	}
};
