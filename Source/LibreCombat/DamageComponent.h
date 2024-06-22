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
	// Sets default values for this component's properties
	UDamageComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float InitialHealth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Health;

	void OnTakeAnyDamage(
		AActor* DamagedActor, 
		float Damage, 
		const class UDamageType* DamageType, 
		class AController* InstigatedBy, 
		AActor* DamageCauser
	);

	UFUNCTION(BlueprintCallable)
	void ReceiveDamage(float DamageAmount);
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UNiagaraSystem* DeathEffect;

	UFUNCTION(BlueprintCallable)
	void Respawn();

	FTimerHandle RespawnTimer;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
