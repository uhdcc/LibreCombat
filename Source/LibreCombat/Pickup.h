// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Pickup.generated.h"

class AWeapon;
class UProjectileMovementComponent;
class USphereComponent;
class USkeletalMeshComponent;
class UCapsuleComponent;

UENUM(BlueprintType)
enum class EPickupTimerMode : uint8 {
	WhenMatchStarts,
	WhenPickedUp
};

UCLASS()
class LIBRECOMBAT_API APickup : public AActor {
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:	
	APickup();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UProjectileMovementComponent* PickupMovement;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USphereComponent* PickupTrigger;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USkeletalMeshComponent* PickupModel;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UCapsuleComponent* PickupCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bNeedToHoldButtonToPickUp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RespawnTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EPickupTimerMode TimerMode;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bWasDropped;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ApplyPickupEffect(AActor* PickerUpper);
	virtual void ApplyPickupEffect_Implementation(AActor* PickerUpper) {};
	UFUNCTION(BlueprintCallable)
	void Respawn();
};

UCLASS()
class LIBRECOMBAT_API AWeaponPickup : public APickup {
	GENERATED_BODY()
protected:
	virtual void OnConstruction(const FTransform& Transform) override;
public:
	void ApplyPickupEffect_Implementation(AActor* PickerUpper) override;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftClassPtr<AWeapon> WeaponClass;
};