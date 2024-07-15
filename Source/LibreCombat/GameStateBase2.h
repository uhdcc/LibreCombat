// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "GameStateBase2.generated.h"

UCLASS()
class LIBRECOMBAT_API AGameStateBase2 : public AGameStateBase {
	GENERATED_BODY()
public:
	virtual void HandleBeginPlay() override;
	virtual void OnRep_ReplicatedHasBegunPlay() override;

#if WITH_EDITOR
	void AllWeaponsLoaded();
#endif
	void AllPickupsLoaded();
};
