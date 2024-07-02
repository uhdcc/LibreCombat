// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerController2.h"

APlayerController2::APlayerController2() {
	PrimaryActorTick.bCanEverTick = true;
	bInputsHaveBeenBinded = false;
	bIsZooming = false;
	ZoomTimer = 0.f;
	FovGoal = 0.f;
	InitialSensitivity = 0.f;
	InitialFov = InitialFov;
}

void APlayerController2::BeginPlay() {
	Super::BeginPlay();
	InitialSensitivity = 16.5f;
	SetMouseSensitivity(800.0, InitialSensitivity);
	InitialFov = 114.5916f;
	PlayerCameraManager->DefaultFOV = InitialFov;
	PlayerCameraManager->bDefaultConstrainAspectRatio = true;
	PlayerCameraManager->DefaultAspectRatio = 1.777777f;
}

void APlayerController2::SetupInputComponent() {
	Super::SetupInputComponent();
}

void APlayerController2::SetPawn(APawn* InPawn) {
	Super::SetPawn(InPawn);
	if (!bInputsHaveBeenBinded && GetPawn()) {
		InputComponent->BindAxis("MouseYaw", this, &APlayerController::AddYawInput);
		InputComponent->BindAxis("MousePitch", this, &APlayerController::AddPitchInput);
		InputComponent->BindAxis("MoveForward", this, &APlayerController2::MoveForward);
		InputComponent->BindAxis("MoveRight", this, &APlayerController2::MoveRight);
		bInputsHaveBeenBinded = true;
	}
}

void APlayerController2::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);
	if (bIsZooming) {
		ZoomTimer -= DeltaSeconds;
		if (ZoomTimer > 0.f) {
			auto Alpha = ZoomTimer / 0.1f;
			PlayerCameraManager->DefaultFOV = FMath::Lerp(FovGoal, PlayerCameraManager->DefaultFOV, Alpha);
		}
		else{
			PlayerCameraManager->DefaultFOV = FovGoal;
			bIsZooming = false;
		}
	}	
}

void APlayerController2::MoveForward(float Input) {
	if(Input != 0.f) GetPawn()->AddMovementInput(GetPawn()->GetActorForwardVector(), Input);
}
void APlayerController2::MoveRight(float Input) {
	if(Input != 0.f) GetPawn()->AddMovementInput(GetPawn()->GetActorRightVector(), Input);
}
void APlayerController2::SetMouseSensitivity(double MouseDpi, double CentimetersPer360) {
	auto MouseSensitivity = 360.0 / MouseDpi / CentimetersPer360 * 2.54;
	InputYawScale = MouseSensitivity;
	InputPitchScale = MouseSensitivity;
}

void APlayerController2::Zoom(float NewFov, float NewSensitivity) {
	FovGoal = NewFov;
	SetMouseSensitivity(800.0, NewSensitivity);
	ZoomTimer = 0.1f;
	bIsZooming = true;
}

void APlayerController2::UnZoom() {
	FovGoal = InitialFov;
	SetMouseSensitivity(800.0, InitialSensitivity);
	ZoomTimer = 0.1f;
	bIsZooming = true;
}
