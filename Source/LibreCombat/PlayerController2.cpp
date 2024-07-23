// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerController2.h"

APlayerController2::APlayerController2() {
	PrimaryActorTick.bCanEverTick = true;
	bInputsHaveBeenBinded = false;

	bIsZooming = false;
	FovGoal = 0.f;
	InitialSensitivity = 0.f;
	InitialFov = 0.f;
	ZoomedSensitivity = 1.0;
	bIsZoomed = false;
	ZoomTimer = 0.1f;

}
void APlayerController2::BeginPlay() {
	Super::BeginPlay();
	if (IsLocalPlayerController()) {
		InitialSensitivity = 16.5f;
		InitialFov = 110;
		PlayerCameraManager->DefaultFOV = InitialFov;
		PlayerCameraManager->bDefaultConstrainAspectRatio = true;
		PlayerCameraManager->DefaultAspectRatio = 1.777777f;

		//SetMouseSensitivity(50.0, InitialSensitivity);
		SetMouseSensitivity(800.0, InitialSensitivity);
		//SetMouseSensitivity(800.0, 20);
		//SetMouseSensitivity(26000.0, InitialSensitivity);
		SetZoomedSensitivity(800.0, InitialSensitivity / 2.0);

		MouseRotationX = ControlRotation.Yaw / MouseSensitivity;
		MouseRotationY = ControlRotation.Pitch / MouseSensitivity;

	}
}
void APlayerController2::SetupInputComponent() {
	Super::SetupInputComponent();
}
void APlayerController2::SetPawn(APawn* InPawn) {
	Super::SetPawn(InPawn);
	if (IsLocalPlayerController()) {
		if (!bInputsHaveBeenBinded && GetPawn()) {
			//InputComponent->BindAxis("MouseYaw", this, &APlayerController::AddYawInput);
			//InputComponent->BindAxis("MousePitch", this, &APlayerController::AddPitchInput);
			InputComponent->BindAxis("MoveForward", this, &APlayerController2::MoveForward);
			InputComponent->BindAxis("MoveRight", this, &APlayerController2::MoveRight);

			FWindowsApplication* WindowsApplication = (FWindowsApplication*)FSlateApplication::Get().GetPlatformApplication().Get();
			check(WindowsApplication);
			WindowsApplication->AddMessageHandler(*this);

			bInputsHaveBeenBinded = true;
		}
	}
}
void APlayerController2::Destroyed() {
	FWindowsApplication* WindowsApplication = (FWindowsApplication*)FSlateApplication::Get().GetPlatformApplication().Get();
	check(WindowsApplication);
	WindowsApplication->RemoveMessageHandler(*this);
	Super::Destroyed();
}
void APlayerController2::Tick(float DeltaSeconds) {
	if (bHasMouseInput) {
		ControlRotation = FRotator(
			(MouseRotationY * MouseSensitivity) + (ZoomedMouseRotationY * ZoomedSensitivity),
			(MouseRotationX * MouseSensitivity) + (ZoomedMouseRotationX * ZoomedSensitivity),
			0.f
		);
		bHasMouseInput = false;
	}
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
	Super::Tick(DeltaSeconds);
}
void APlayerController2::MoveForward(float Input) {
	if(Input != 0.f) GetPawn()->AddMovementInput(GetPawn()->GetActorForwardVector(), Input);
}
void APlayerController2::MoveRight(float Input) {
	if(Input != 0.f) GetPawn()->AddMovementInput(GetPawn()->GetActorRightVector(), Input);
}
void APlayerController2::Zoom(float NewFov, float NewSensitivity) {
	FovGoal = NewFov;
	ZoomTimer = 0.1f;
	bIsZooming = true;

	ZoomedSensitivity = 360.0 / 800.0 / NewSensitivity * 2.54;
	bIsZoomed = true;
}
void APlayerController2::UnZoom() {
	FovGoal = InitialFov;
	bIsZooming = true;

	bIsZoomed = false;
}
bool APlayerController2::ProcessMessage(HWND hwnd, uint32 msg, WPARAM wParam, LPARAM lParam, int32& OutResult) {
	switch (msg) {
	case WM_INPUT:
	{
		uint32 Size = 0;
		GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &Size, sizeof(RAWINPUTHEADER));
		TUniquePtr<uint8[]> RawData = MakeUnique<uint8[]>(Size);
		if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, RawData.Get(), &Size, sizeof(RAWINPUTHEADER)) == Size) {
			const RAWINPUT* const Raw = (const RAWINPUT* const)RawData.Get();
			if (Raw->header.dwType == RIM_TYPEMOUSE) {
				if (!(Raw->data.mouse.usFlags & MOUSE_MOVE_ABSOLUTE)) {
					if (Raw->data.mouse.lLastX != 0) {
						if (bIsZoomed) {
							ZoomedMouseRotationX += Raw->data.mouse.lLastX;
						}
						else {
							MouseRotationX += Raw->data.mouse.lLastX;
						}
						bHasMouseInput = true;
					}
					if (Raw->data.mouse.lLastY != 0) {
						if (bIsZoomed) {
							ZoomedMouseRotationY -= Raw->data.mouse.lLastY;
						}
						else {
							MouseRotationY -= Raw->data.mouse.lLastY;
						}
						bHasMouseInput = true;
					}
				}
			}
		}
		break;
	}
	//case WM_KEYDOWN:
	//{
	//	uint32 CharCode = MapVirtualKey(wParam, MAPVK_VK_TO_CHAR);
	//	auto PressedKey = FInputKeyManager::Get().GetKeyFromCodes(wParam, CharCode);
	//	if (SubFrameTriggers.Contains(PressedKey)) {
	//		if (!BroadcastSubframeOnNextTick) {
	//			PreviousLocation = PlayerCameraManager->GetCameraLocation();
	//			SubframeTimestamp = FPlatformTime::Seconds();
	//			SubframeYaw = MouseRotationX;
	//			SubframePitch = MouseRotationY;
	//			//if (abs(SubframePitch) > 1300)
	//			//{
	//			//	SubframePitch = copysign(1300, SubframePitch);
	//			//}
	//			PressedKeys.Add(PressedKey);
	//			BroadcastSubframeOnNextTick = true;
	//		}
	//	}
	//	break;
	//}
	}
	return false;
}

void APlayerController2::SetMouseSensitivity(double MouseDpi, double CentimetersPer360) {
	MouseSensitivity = 360.0 / MouseDpi / CentimetersPer360 * 2.54;
	//MouseSensitivityRadians = MouseSensitivity * DegreesToRadians;
	//InputYawScale = MouseSensitivity;
	//InputPitchScale = MouseSensitivity;

}

void APlayerController2::SetZoomedSensitivity(double MouseDpi, double CentimetersPer360) {
	ZoomedSensitivity = 360.0 / MouseDpi / CentimetersPer360 * 2.54;
}
