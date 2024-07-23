// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeaponStructs.h"
#include "DamageComponent.generated.h"

class UNiagaraSystem;
class AHUD2;

USTRUCT(BlueprintType)
struct FRechargeParameters {
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RechargeGoal;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RechargeRate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RechargeDelay;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCanBeInterrupted;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RechargeTimestamp;

	FRechargeParameters() {
		RechargeGoal = 0.f;
		RechargeRate = 0.f;
		RechargeDelay = 0.f;
		bCanBeInterrupted = true;
		RechargeTimestamp = 0.f;
	}
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LIBRECOMBAT_API UDamageComponent : public UActorComponent {
	GENERATED_BODY()
protected:
	float Health;
	float Shield;
	virtual void BeginPlay() override;
public:	
	UDamageComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float InitialHealth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bActorIsDead;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bActorIsImmortal;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bPostDeathHasBeenTicked;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float InitialShield;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bShieldIsBroken;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bShieldIsRecharging;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RechargeStartingShield;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ElapsedRechargeTime;

	UFUNCTION(BlueprintCallable)
	void ReceiveDamage(const FDamageParameters& Parameters);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UNiagaraSystem* DeathEffect;
	UFUNCTION(BlueprintCallable)
	void Respawn();
	FTimerHandle RespawnTimer;

	UFUNCTION(BlueprintCallable)
	void StartRecharging(const FRechargeParameters& Parameters);
	UFUNCTION(BlueprintCallable)
	void StopRecharging();
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRechargeParameters CurrentRecharge;
	float GetTimestamp();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AHUD2* HUD2;

	UFUNCTION(BlueprintCallable)
	void SetHealth(float NewHealth);
	UFUNCTION(BlueprintCallable)
	void SetShield(float NewShield);
	UFUNCTION(BlueprintPure)
	float GetHealth();
	UFUNCTION(BlueprintPure)
	float GetShield();
};
