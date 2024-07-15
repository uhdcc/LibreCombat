// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeaponStructs.h"
#include "WeaponHolder.generated.h"

class AWeapon;
class APickup;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LIBRECOMBAT_API UWeaponHolder : public UActorComponent
{
	GENERATED_BODY()
protected:
	virtual void BeginPlay() override;
public:	
	UWeaponHolder();

	UFUNCTION(BlueprintCallable)
	void Interact(bool bButtonWasPressed);
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bInteractButtonIsHeld;
	UFUNCTION()
	void OnOverlap(AActor* OverlappedActor, AActor* OtherActor);
	UFUNCTION()
	void OnEndOverlap(AActor* OverlappedActor, AActor* OtherActor);
	UFUNCTION(BlueprintCallable)
	void InteractEvent();

	
	UFUNCTION(BlueprintCallable)
	void CycleWeapon(bool bDirection);
	UFUNCTION(BlueprintCallable)
	void SelectWeapon(int WeaponIndex);
	UFUNCTION(BlueprintCallable)
	void ThrowGrenade();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<AWeapon*> Weapons;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int CurrentWeaponIndex;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FProjectileSpawnParameters GrenadeSpawnParams;

	UFUNCTION(BlueprintCallable)
	bool ReplaceCurrentWeapon(TSubclassOf<AWeapon> NewWeaponClass);
	UFUNCTION(BlueprintCallable)
	void DropPickup(TSubclassOf<APickup> PickupClass, AActor* PickupOwner);
	UFUNCTION(BlueprintCallable)
	int AddWeapon(AWeapon* NewWeapon);

	bool bWeaponSwitchingHasBeenBinded;
};
