// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Character2.generated.h"

UCLASS()
class OPENCOMBAT_API ACharacter2 : public ACharacter
{
	GENERATED_BODY()
public:
	ACharacter2();

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode = 0) override;

	virtual void Jump() override;
	virtual void StopJumping() override;
	bool isHoldingJumpButton;
};
