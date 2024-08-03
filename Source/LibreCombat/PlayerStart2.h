// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerStart.h"
#include "PlayerStart2.generated.h"

/**
 * 
 */
UCLASS()
class LIBRECOMBAT_API APlayerStart2 : public APlayerStart
{
	GENERATED_BODY()
public:
	virtual void BeginPlay() override;

};
