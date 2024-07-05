// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/ActorComponent.h"
#include "Weapon.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FButtonEvent, UButtonComponent*, Button);


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
class LIBRECOMBAT_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	AWeapon();
	virtual void BeginPlay() override;

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

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Equip();
	void Equip_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Unequip();
	void Unequip_Implementation();
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsEquipped;

};
