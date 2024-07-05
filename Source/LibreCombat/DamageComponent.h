// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DamageComponent.generated.h"

class UNiagaraSystem;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LIBRECOMBAT_API UDamageComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UDamageComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float InitialHealth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Health;

	UFUNCTION(BlueprintCallable)
	void ReceiveDamage(float DamageAmount);
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UNiagaraSystem* DeathEffect;

	UFUNCTION(BlueprintCallable)
	void Respawn();

	FTimerHandle RespawnTimer;

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
