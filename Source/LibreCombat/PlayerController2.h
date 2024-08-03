// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Windows/WindowsApplication.h"

#include "PlayerController2.generated.h"

#define Pi 3.14159265358979
#define DegreesToRadians 0.01745329251994
#define RadiansToDegrees 57.2957795130823

UCLASS()
class LIBRECOMBAT_API APlayerController2 : public APlayerController, public IWindowsMessageHandler
{
	GENERATED_BODY()
public:
	APlayerController2();

	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void SetPawn(APawn* InPawn) override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void Destroyed() override;

	bool bInputsHaveBeenBinded;

	void MoveForward(float Input);
	void MoveRight(float Input);
	
	bool bIsZooming;
	float ZoomTimer;
	float FovGoal;
	float InitialFov;
	float InitialSensitivity;
	void Zoom(float NewFov, float NewSensitivity);
	void UnZoom();

	virtual bool ProcessMessage(HWND hwnd, uint32 msg, WPARAM wParam, LPARAM lParam, int32& OutResult) override;
	void SetMouseSensitivity(double MouseDpi, double CentimetersPer360);

	double MouseSensitivity;
	double ZoomedSensitivity;
	long MouseRotationX;
	long MouseRotationY;
	bool bHasMouseInput;
	long ZoomedMouseRotationX;
	long ZoomedMouseRotationY;
	bool bIsZoomed;	
};
