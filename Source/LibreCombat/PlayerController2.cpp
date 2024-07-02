// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerController2.h"

APlayerController2::APlayerController2() {
}

void APlayerController2::BeginPlay() {
	Super::BeginPlay();
	SetMouseSensitivity(800.0, 16.5);
	PlayerCameraManager->DefaultFOV = 114.5f;
	PlayerCameraManager->bDefaultConstrainAspectRatio = true;
	PlayerCameraManager->DefaultAspectRatio = 1.777777f;
	bInputsHaveBeenBinded = false;
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
