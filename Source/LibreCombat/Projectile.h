// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;
class UNiagaraSystem;


UCLASS()
class LIBRECOMBAT_API AProjectile : public AActor {
	GENERATED_BODY()	
public:	
	AProjectile();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USphereComponent* ProjectileCollision;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* Model;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UProjectileMovementComponent* Movement;	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* BounceSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* ExplosionSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* HitSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UNiagaraSystem* ExplosionEffect;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ExplosionRadius;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ExplosionForce;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ExplosionDamage;

	UFUNCTION()
	void Bounce(const FHitResult& ImpactResult, const FVector& ImpactVelocity);
	UFUNCTION()
	void Stopped(const FHitResult& ImpactResult);


	void Explode();

protected:
	virtual void BeginPlay() override;


};
