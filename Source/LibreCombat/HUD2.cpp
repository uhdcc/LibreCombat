// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD2.h"
#include "SlateOptMacros.h"
#include "Widgets/Layout/SConstraintCanvas.h"
#include "Widgets/Images/SImage.h"

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
	if (GetOwningPlayerController()) {
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
	}
	Super::BeginPlay();
}
void AHUD2::SetReticleImage(UObject* NewImage, FVector2D NewSize) {
	if (Reticle && NewImage) {
		Reticle->ReticleSlateBrush->SetResourceObject(NewImage);
		Reticle->ReticleSlateBrush->ImageSize = NewSize;
		Reticle->ReticleImage->SetImage(Reticle->ReticleSlateBrush.Get());
	}
}
