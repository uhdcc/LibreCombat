// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD2.h"
#include "SlateOptMacros.h"
#include "Widgets/Layout/SConstraintCanvas.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Layout/SSpacer.h"
#include "Weapon.h"
#include "Engine/CanvasRenderTarget2D.h"
#include "Engine/Canvas.h"


BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SWeaponListWidget::Construct(const FArguments& InArgs) {
	ChildSlot
		[
			SAssignNew(VerticalBox, SVerticalBox)
		];
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION
void AHUD2::AddWeaponToList(FWeaponHudParameters Parameters) {
	WeaponList->VerticalBox->AddSlot()
		[
			SNew(SBorder)
				[
					SNew(STextBlock)
						.Text(Parameters.WeaponName)
						.Font(FCoreStyle::GetDefaultFontStyle("Regular", 22))
				]
		];
}
void AHUD2::RemoveWeaponFromList(FWeaponHudParameters Parameters) {
	FChildren* SharedChildren = WeaponList->VerticalBox->GetChildren();
	for (int32 i = 0; i < SharedChildren->Num(); ++i) {
		TSharedRef<SWidget> Child = SharedChildren->GetChildAt(i);
		TSharedRef<SBorder> Border = StaticCastSharedRef<SBorder>(Child);
		TSharedRef<STextBlock> TextBlock = StaticCastSharedRef<STextBlock>(Border->GetContent());
		if (TextBlock->GetText().EqualTo(Parameters.WeaponName)) {
			WeaponList->VerticalBox->RemoveSlot(Child);
			break;
		}
	}
}
void AHUD2::OnEquipWeapon(FWeaponHudParameters Parameters) {
	// change reticle
	//SetReticleImage(Parameters.ReticleTexture, Parameters.ReticleSize);
	// update ammo
	if (Parameters.Weapon) {
		if (Parameters.Weapon->bHasAmmo) {
			AmmoWidget->SetVisibility(EVisibility::Visible);
			SetAmmo(Parameters.Weapon->Ammo.Magazine, Parameters.Weapon->Ammo.Bandolier);
		}
		else {
			AmmoWidget->SetVisibility(EVisibility::Hidden);
		}
	}
	// change picture

	// highlight name on list
	FChildren* SharedChildren = WeaponList->VerticalBox->GetChildren();
	for (int32 i = 0; i < SharedChildren->Num(); ++i) {
		TSharedRef<SWidget> Child = SharedChildren->GetChildAt(i);
		TSharedRef<SBorder> Border = StaticCastSharedRef<SBorder>(Child);
		TSharedRef<STextBlock> TextBlock = StaticCastSharedRef<STextBlock>(Border->GetContent());
		if (TextBlock->GetText().EqualTo(Parameters.WeaponName)) {
			Border->SetBorderBackgroundColor(FLinearColor(0.12f, 0.12f, 0.12f, 1.f));
			TextBlock->SetColorAndOpacity(FLinearColor::Red);
		}
		else {
			Border->SetBorderBackgroundColor(FLinearColor::Transparent);
			TextBlock->SetColorAndOpacity(FLinearColor::White);
		}
	}
}
void AHUD2::BeginPlay() {
	Super::BeginPlay();

	auto ReticleCanvas = NewObject<UCanvas>(this, "ReticleCanvas");
	ReticleTexture = NewObject<UCanvasRenderTarget2D>(this, "ReticleTexture");
	Reticle->DrawCrosshair(FCrossProperties(), ReticleCanvas, ReticleTexture);
}
void AHUD2::PostInitializeComponents() {
	Super::PostInitializeComponents();

	GEngine->GameViewport->AddViewportWidgetForPlayer(
		GetOwningPlayerController()->GetLocalPlayer(),
		SAssignNew(PlayerSlateHud, SConstraintCanvas),
		0
	);
	PlayerSlateHud->AddSlot()
		.Anchors(FAnchors(0.5f, 0.5f, 0.5f, 0.5f))
		.Alignment(FVector2D(0.5f))
		.AutoSize(true)
		[
			SAssignNew(Reticle, SReticleWidget)
		];
	PlayerSlateHud->AddSlot()
		.Anchors(FAnchors(1.f, 0.5f, 1.f, 0.5f))
		.Alignment(FVector2D(1.f, 0.5f))
		.AutoSize(true)
		[
			SAssignNew(WeaponList, SWeaponListWidget)
		];

	NumberFormat.SetMaximumFractionalDigits(0);
	PlayerSlateHud->AddSlot()
		.Anchors(FAnchors(0.5f, 1.f, 0.5f, 1.f))
		.Alignment(FVector2D(0.5f, 1.f))
		.AutoSize(true)
		[
			SAssignNew(HealthShield, SHealthWidget)
		];
	PlayerSlateHud->AddSlot()
		.Anchors(FAnchors(0.f, 0.f, 0.f, 0.f))
		.Alignment(FVector2D(0.f, 0.f))
		.AutoSize(true)
		[
			SAssignNew(AmmoWidget, SAmmoWidget)
		];
	PlayerSlateHud->AddSlot()
		.Anchors(FAnchors(0.f,1.f, 0.f, 1.f))
		.Alignment(FVector2D(0.f, 1.f))
		.AutoSize(true)
		[
			SAssignNew(ClockWidget, SClockWidget)
		];
}
void AHUD2::SetReticleImage(UObject* NewImage, FVector2D NewSize) {
	if (Reticle && NewImage) {
		Reticle->ReticleSlateBrush->SetResourceObject(NewImage);
		Reticle->ReticleSlateBrush->ImageSize = NewSize;
		Reticle->ReticleImage->SetImage(Reticle->ReticleSlateBrush.Get());
	}
}
void SHealthWidget::Construct(const FArguments& InArgs) {
	FSlateFontInfo BigHudFont = FCoreStyle::GetDefaultFontStyle("Bold", 70);
	BigHudFont.OutlineSettings.OutlineSize = 2;
	ChildSlot
		[
			SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()				
				.AutoWidth()
				[
					SAssignNew(Health, STextBlock)
						.Text(FText::FromString("999"))
						.Font(BigHudFont)
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SSpacer)
						.Size(FVector2D(50.f, 1.f))
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SAssignNew(Shield, STextBlock)
						.Text(FText::FromString("999"))
						.Font(BigHudFont)
				]
		];
}
void AHUD2::SetHealth(float NewHealth) {
	float Alpha = InitialHealth * 0.1f;
	Alpha =(NewHealth - Alpha) / (InitialHealth - Alpha);
	FLinearColor NewColor = FMath::Lerp(FLinearColor::Red, FLinearColor::Green, Alpha);
	HealthShield->Health->SetText(FText::AsNumber(NewHealth, &NumberFormat));
	HealthShield->Health->SetColorAndOpacity(NewColor);
}
void AHUD2::SetShield(float NewShield) {
	float Alpha = InitialShield * 0.1f;
	Alpha = (NewShield - Alpha) / (InitialShield - Alpha);
	FLinearColor NewColor = FMath::Lerp(FLinearColor::Red, FLinearColor::Blue, Alpha);
	HealthShield->Shield->SetText(FText::AsNumber(NewShield, &NumberFormat));
	HealthShield->Shield->SetColorAndOpacity(NewColor);
}
void AHUD2::SetAmmo(float NewMagazine, float NewBandolier) {
	AmmoWidget->Magazine->SetText(FText::AsNumber(NewMagazine, &NumberFormat));
	AmmoWidget->Bandolier->SetText(FText::AsNumber(NewBandolier, &NumberFormat));
}
void AHUD2::SetTime(float NewTime) {	
	ClockWidget->ClockText->SetText(FText::AsTimespan(FTimespan::FromSeconds(NewTime)));
}
void SAmmoWidget::Construct(const FArguments& InArgs) {
	FSlateFontInfo BigHudFont = FCoreStyle::GetDefaultFontStyle("Bold", 30);
	BigHudFont.OutlineSettings.OutlineSize = 2;
	FSlateFontInfo BigHudFont2 = FCoreStyle::GetDefaultFontStyle("Bold", 40);
	BigHudFont2.OutlineSettings.OutlineSize = 2;
	ChildSlot
	[
		SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			[
				SAssignNew(Magazine, STextBlock)
					.Text(FText::FromString("99"))
					.Font(BigHudFont)
			]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
					.Text(FText::FromString("/"))
					.Font(BigHudFont2)
			]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			[
				SAssignNew(Bandolier, STextBlock)
					.Text(FText::FromString("999"))
					.Font(BigHudFont)
			]
	];
}
void SClockWidget::Construct(const FArguments& InArgs) {
	FSlateFontInfo BigHudFont2 = FCoreStyle::GetDefaultFontStyle("Bold", 70);
	BigHudFont2.OutlineSettings.OutlineSize = 2;
	ChildSlot
		[
			SAssignNew(ClockText, STextBlock)
				.Text(FText::FromString("999"))
				.Font(BigHudFont2)
		];
}
