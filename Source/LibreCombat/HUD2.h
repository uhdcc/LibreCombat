// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Widgets/SCompoundWidget.h"
#include "WeaponStructs.h"
#include "HUD2.generated.h"

class SConstraintCanvas;

class LIBRECOMBAT_API SHealthShield : public SCompoundWidget {
public:
	SLATE_BEGIN_ARGS(SHealthShield) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
	TSharedPtr<SHorizontalBox> HorizontalBox;
	TSharedPtr<STextBlock> Health;
	TSharedPtr<STextBlock> Shield;

};
class LIBRECOMBAT_API SWeaponList : public SCompoundWidget {
public:
	SLATE_BEGIN_ARGS(SWeaponList) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
	TSharedPtr<SVerticalBox> VerticalBox;
};
class LIBRECOMBAT_API SReticle : public SCompoundWidget {
public:
	SLATE_BEGIN_ARGS(SReticle) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
	TSharedPtr<SImage> ReticleImage;
	TSharedPtr<FSlateBrush> ReticleSlateBrush;
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
	TSharedPtr<SReticle> Reticle;
	TSharedPtr<SWeaponList> WeaponList;
	TSharedPtr<SHealthShield> HealthShield;
	FNumberFormattingOptions NumberFormat;



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
};
