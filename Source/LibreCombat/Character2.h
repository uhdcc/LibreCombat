// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Navigation/PathFollowingComponent.h"
#include "CharacterMovementComponent2.h"
#include "Character2.generated.h"

class UWeaponHolder;
class UDamageComponent;

UCLASS()
class LIBRECOMBAT_API ACharacter2 : public ACharacter
{
	GENERATED_BODY()
public:
	ACharacter2(const FObjectInitializer& ObjectInitializer);
	virtual void BeginPlay() override;
	virtual void RecalculateBaseEyeHeight() override;
	virtual void CalcCamera(float DeltaTime, struct FMinimalViewInfo& OutResult);
	virtual void FaceRotation(FRotator NewControlRotation, float DeltaTime = 0.f);

	UFUNCTION(BlueprintCallable)
	void Ragdoll();
	UFUNCTION(BlueprintCallable)
	void ReverseRagdoll();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* Eyes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCharacterMovementComponent2* CharacterMovement2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UDamageComponent* DamageComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UWeaponHolder* WeaponHolder;

	UFUNCTION()
	void MoveToRandomPoint(EPathFollowingResult::Type MovementResult);
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bDontMove;

	UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* FirstPersonMesh;
};
