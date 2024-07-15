// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD2.h"
#include "SlateOptMacros.h"
#include "Widgets/Layout/SConstraintCanvas.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Layout/SSpacer.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SWeaponList::Construct(const FArguments& InArgs) {
	ChildSlot
		[
			SAssignNew(VerticalBox, SVerticalBox)
		];
}
//void SWeaponList::Highlight() {
//}
//void SWeaponList::Unhighlight() {
//}
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
	SetReticleImage(Parameters.ReticleTexture, Parameters.ReticleSize);
	// update ammo

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
void SReticle::Construct(const FArguments& InArgs) {
	ReticleSlateBrush = MakeShareable(new FSlateBrush(*FCoreStyle::Get().GetBrush("GenericWhiteBoxBrush")));
	ReticleSlateBrush->Tiling = ESlateBrushTileType::NoTile;
	ChildSlot
		[
			SAssignNew(ReticleImage, SImage)
				.Image(ReticleSlateBrush.Get())
		];
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION
void AHUD2::BeginPlay() {
	Super::BeginPlay();
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
			SAssignNew(Reticle, SReticle)
		];
	PlayerSlateHud->AddSlot()
		.Anchors(FAnchors(1.f, 0.5f, 1.f, 0.5f))
		.Alignment(FVector2D(1.f, 0.5f))
		.AutoSize(true)
		[
			SAssignNew(WeaponList, SWeaponList)
		];

	NumberFormat.SetMaximumFractionalDigits(0);
	PlayerSlateHud->AddSlot()
		.Anchors(FAnchors(0.5f, 1.f, 0.5f, 1.f))
		.Alignment(FVector2D(0.5f, 1.f))
		.AutoSize(true)
		[
			SAssignNew(HealthShield, SHealthShield)
		];
}
void AHUD2::SetReticleImage(UObject* NewImage, FVector2D NewSize) {
	if (Reticle && NewImage) {
		Reticle->ReticleSlateBrush->SetResourceObject(NewImage);
		Reticle->ReticleSlateBrush->ImageSize = NewSize;
		Reticle->ReticleImage->SetImage(Reticle->ReticleSlateBrush.Get());
	}
}
void SHealthShield::Construct(const FArguments& InArgs) {
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

	//float Alpha = (NewHealth - 20.f) / (InitialHealth - 20.f);
	FLinearColor NewColor = FMath::Lerp(FLinearColor::Red, FLinearColor::Green, Alpha);
	HealthShield->Health->SetText(FText::AsNumber(NewHealth, &NumberFormat));
	HealthShield->Health->SetColorAndOpacity(NewColor);
}
void AHUD2::SetShield(float NewShield) {
	float Alpha = InitialShield * 0.1f;
	Alpha = (NewShield - Alpha) / (InitialShield - Alpha);

	//float Alpha = (NewShield - 20.f) / (InitialShield - 20.f);
	FLinearColor NewColor = FMath::Lerp(FLinearColor::Red, FLinearColor::Blue, Alpha);
	HealthShield->Shield->SetText(FText::AsNumber(NewShield, &NumberFormat));
	HealthShield->Shield->SetColorAndOpacity(NewColor);
}
