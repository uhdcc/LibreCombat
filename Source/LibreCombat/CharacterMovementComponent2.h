// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CharacterMovementComponent2.generated.h"

/**
 * 
 */
UCLASS()
class LIBRECOMBAT_API UCharacterMovementComponent2 : public UCharacterMovementComponent
{
	GENERATED_BODY()
public:
	UCharacterMovementComponent2();
	virtual float GetMaxSpeed() const override;
	bool bUseCrouchWalkingSpeed;
	
};
