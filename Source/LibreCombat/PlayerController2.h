// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PlayerController2.generated.h"

UCLASS()
class LIBRECOMBAT_API APlayerController2 : public APlayerController
{
	GENERATED_BODY()
public:
	APlayerController2();

	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void SetPawn(APawn* InPawn) override;
	virtual void Tick(float DeltaSeconds) override;

	bool bInputsHaveBeenBinded;

	void MoveForward(float Input);
	void MoveRight(float Input);

	void SetMouseSensitivity(double MouseDpi, double CentimetersPer360);
	
	bool bIsZooming;
	float ZoomTimer;
	float FovGoal;
	float InitialFov;
	float InitialSensitivity;
	void Zoom(float NewFov, float NewSensitivity);
	void UnZoom();
};
