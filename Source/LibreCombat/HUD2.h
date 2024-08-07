// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Widgets/SCompoundWidget.h"
#include "WeaponStructs.h"
#include "Components/Widget.h"
#include "SReticle.h"
#include "HUD2.generated.h"

class SConstraintCanvas;

class LIBRECOMBAT_API SClockWidget : public SCompoundWidget {
public:
	SLATE_BEGIN_ARGS(SClockWidget) {}
	SLATE_END_ARGS()
	void Construct(const FArguments& InArgs);
	TSharedPtr<STextBlock> ClockText;
};
class LIBRECOMBAT_API SAmmoWidget : public SCompoundWidget {
public:
	SLATE_BEGIN_ARGS(SAmmoWidget) {}
	SLATE_END_ARGS()
	void Construct(const FArguments& InArgs);
	TSharedPtr<SHorizontalBox> HorizontalBox;
	TSharedPtr<STextBlock> Magazine;
	TSharedPtr<STextBlock> Bandolier;
};
class LIBRECOMBAT_API SHealthWidget : public SCompoundWidget {
public:
	SLATE_BEGIN_ARGS(SHealthWidget) {}
	SLATE_END_ARGS()
	void Construct(const FArguments& InArgs);
	TSharedPtr<SHorizontalBox> HorizontalBox;
	TSharedPtr<STextBlock> Health;
	TSharedPtr<STextBlock> Shield;
};
class LIBRECOMBAT_API SWeaponListWidget : public SCompoundWidget {
public:
	SLATE_BEGIN_ARGS(SWeaponListWidget) {}
	SLATE_END_ARGS()
	void Construct(const FArguments& InArgs);
	TSharedPtr<SVerticalBox> VerticalBox;
};
UCLASS()
class LIBRECOMBAT_API AHUD2 : public AHUD {
	GENERATED_BODY()
public:
	float InitialHealth;
	float InitialShield;
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;
	TSharedPtr<SConstraintCanvas> PlayerSlateHud;
	TSharedPtr<SReticleWidget> Reticle;
	TSharedPtr<SWeaponListWidget> WeaponList;
	TSharedPtr<SHealthWidget> HealthShield;
	FNumberFormattingOptions NumberFormat;
	TSharedPtr<SAmmoWidget> AmmoWidget;
	TSharedPtr<SClockWidget> ClockWidget;
	UFUNCTION(BlueprintCallable)
	void SetReticleImage(UObject* NewImage, FVector2D NewSize);
	UFUNCTION(BlueprintCallable)
	void AddWeaponToList(FWeaponHudParameters Parameters);
	UFUNCTION(BlueprintCallable)
	void RemoveWeaponFromList(FWeaponHudParameters Parameters);
	UFUNCTION(BlueprintCallable)
	void OnEquipWeapon(FWeaponHudParameters Parameters);
	UFUNCTION(BlueprintCallable)
	void SetHealth(float NewHealth);
	UFUNCTION(BlueprintCallable)
	void SetShield(float NewShield);
	UFUNCTION(BlueprintCallable)
	void SetAmmo(float NewMagazine, float NewBandolier);
	UFUNCTION(BlueprintCallable)
	void SetTime(float NewTime);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCanvasRenderTarget2D* ReticleTexture;
};
