// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "HUD2.h"

#include "GameStateBase2.generated.h"

//class AHUD2;

UCLASS()
class LIBRECOMBAT_API AGameStateBase2 : public AGameStateBase {
	GENERATED_BODY()
public:
	virtual void HandleBeginPlay() override;
	virtual void OnRep_ReplicatedHasBegunPlay() override;
	virtual void OnRep_ReplicatedWorldTimeSeconds() override;
	virtual void UpdateServerTimeSeconds() override;

	void AllWeaponsLoaded();
	void AllPickupsLoaded();

	UPROPERTY()
	TArray<AHUD2*> Huds;
};
