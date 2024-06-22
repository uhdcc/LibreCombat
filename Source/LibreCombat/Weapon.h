// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

UCLASS()
class LIBRECOMBAT_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	AWeapon();
	virtual void BeginPlay() override;

	bool bIsEquipped;
	void Shoot(bool bButtonWasPressed);

	void Equip();

	void Unequip();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMeshComponent* FirstPersonMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMeshComponent* ThirdPersonMesh;



	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PhysicsForce;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Damage;
};
