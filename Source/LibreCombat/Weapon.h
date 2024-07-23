// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/ActorComponent.h"
#include "WeaponStructs.h"
#include "Weapon.generated.h"

class AHUD2;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FButtonEvent, UButtonComponent*, Button);

USTRUCT(BlueprintType)
struct FAmmo {
	GENERATED_BODY()
public:
	FAmmo();
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Magazine;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Bandolier;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float InitialMagazine;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float InitialBandolier;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MagazineCapacity;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BandolierCapacity;
};

UCLASS(BlueprintType, Blueprintable, meta = (BlueprintSpawnableComponent))
class UButtonComponent : public UActorComponent {
	GENERATED_BODY()
public: 
	UButtonComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CooldownDuration;
	FTimerHandle CooldownTimer;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsCoolingDown;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bButtonIsHeld;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsEquipped;

	void BindInput(UInputComponent& InputComponent);

	UFUNCTION(BlueprintCallable)
	void Action(bool bButtonWasPressed);
	UFUNCTION(BlueprintCallable)
	void StartCooldown();

	UPROPERTY(BlueprintAssignable)
	FButtonEvent	ButtonEvent;
};
UCLASS()
class LIBRECOMBAT_API AWeapon : public AActor {
	GENERATED_BODY()	
public:	
	AWeapon();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMeshComponent* FirstPersonMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMeshComponent* ThirdPersonMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<AActor*> IgnoredActors;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UButtonComponent*> Buttons;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UButtonComponent* PrimaryFire;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UButtonComponent* SecondaryFire;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UButtonComponent* Reload;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Equip();
	void Equip_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Unequip();
	void Unequip_Implementation();
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsEquipped;

	UPROPERTY()
	AHUD2* HUD2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FWeaponHudParameters HudParameters;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bHasAmmo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FAmmo Ammo;
	UFUNCTION(BlueprintCallable)
	void SubtractAmmo(float SubtractionAmount);
	UFUNCTION(BlueprintCallable)
	void StartReloadingAmmo();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnReloadButtonEvent(UButtonComponent* Button);
	void OnReloadButtonEvent_Implementation(UButtonComponent* Button);
	UFUNCTION(BlueprintCallable)
	void FinishReloadingAmmo();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ReloadDuration;
	FTimerHandle ReloadTimer;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsReloading;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bOutOfAmmo;
};
