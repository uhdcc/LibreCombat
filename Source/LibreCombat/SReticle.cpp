// Fill out your copyright notice in the Description page of Project Settings.


#include "SReticle.h"
#include "SlateOptMacros.h"
#include "Widgets/Images/SImage.h"
#include "Engine/CanvasRenderTarget2D.h"
#include "Engine/Canvas.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Widgets/Layout/SScaleBox.h"


BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SReticleWidget::Construct(const FArguments& InArgs) {
	ReticleSlateBrush = MakeShareable(new FSlateBrush(*FCoreStyle::Get().GetBrush("GenericWhiteBoxBrush")));
	ReticleSlateBrush->Tiling = ESlateBrushTileType::NoTile;
	ChildSlot
	[
		SNew(SScaleBox)
			.IgnoreInheritedScale(true)
			[
				SAssignNew(ReticleImage, SImage)
					.Image(ReticleSlateBrush.Get())
			]
	];
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SReticleWidget::DrawCrosshair(FCrossProperties CrossProperties, UCanvas* ReticleCanvas, UCanvasRenderTarget2D* ReticleTexture) {
	auto bHasOutline = CrossProperties.OutlineThickness > 0.f;
	float ReticleSize = 0.f;
	if (bHasOutline) {
		ReticleSize = (CrossProperties.CrossGap + (CrossProperties.OutlineThickness * 3.f) + CrossProperties.CrossLength) * 2.f;
	}
	else {
		ReticleSize = (CrossProperties.CrossGap + CrossProperties.CrossLength) * 2.f;
	}
	ReticleSize += 1.f;

	ReticleTexture->ClearColor = FLinearColor::Transparent;
	ReticleTexture->LODGroup = TextureGroup::TEXTUREGROUP_Pixels2D;
	ReticleTexture->InitAutoFormat(ReticleSize, ReticleSize);
	ReticleTexture->UpdateResourceImmediate(true);

	auto 	Context = FDrawToRenderTargetContext();
	auto CanvasSize = FVector2D(ReticleSize);
	UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(ReticleCanvas, ReticleTexture, ReticleCanvas, CanvasSize, Context);

	auto Midpoint = FMath::TruncToInt(ReticleSize / 2.f);
	if (bHasOutline) {
		DrawLines(Midpoint, CrossProperties, CrossProperties.CrossThickness + CrossProperties.OutlineThickness * 2.f, ReticleCanvas, CrossProperties.OutlineColor);
	}
	DrawLines(Midpoint, CrossProperties, CrossProperties.CrossThickness, ReticleCanvas, CrossProperties.CrossColor);

	UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(ReticleCanvas, Context);

	ReticleSlateBrush->SetResourceObject(ReticleTexture);
	ReticleSlateBrush->ImageSize = CanvasSize;
	ReticleImage->SetImage(ReticleSlateBrush.Get());
}
void SReticleWidget::DrawLines(float Midpoint, FCrossProperties CrossProperties, float Thickness, UCanvas* ReticleCanvas, FLinearColor Color) {
	auto x = Midpoint + CrossProperties.CrossGap + CrossProperties.OutlineThickness;
	auto y = Midpoint;
	auto FirstPoint = FVector2D(x, y);

	auto x2 = x + CrossProperties.CrossLength;
	auto SecondPoint = FVector2D(x2, y);
	ReticleCanvas->K2_DrawLine(FirstPoint, SecondPoint, Thickness, Color);

	FirstPoint = FVector2D(y, x);
	SecondPoint = FVector2D(y, x2);
	ReticleCanvas->K2_DrawLine(FirstPoint, SecondPoint, Thickness, Color);

	x = Midpoint - CrossProperties.CrossGap - CrossProperties.OutlineThickness;
	FirstPoint = FVector2D(x, y);

	x2 = x - CrossProperties.CrossLength;
	SecondPoint = FVector2D(x2, y);
	ReticleCanvas->K2_DrawLine(FirstPoint, SecondPoint, Thickness, Color);

	FirstPoint = FVector2D(y, x);
	SecondPoint = FVector2D(y, x2);
	ReticleCanvas->K2_DrawLine(FirstPoint, SecondPoint, Thickness, Color);
}
void UReticle::ReleaseSlateResources(bool bReleaseChildren) {
	Super::ReleaseSlateResources(bReleaseChildren);
}
TSharedRef<SWidget> UReticle::RebuildWidget() {
	auto ReticleCanvas = NewObject<UCanvas>(this, "ReticleCanvas");
	if (!ReticleTexture) ReticleTexture = NewObject<UCanvasRenderTarget2D>(this, "ReticleTexture");
	Reticle = SNew(SReticleWidget);
	Reticle->DrawCrosshair(CrossProperties, ReticleCanvas, ReticleTexture);
	return Reticle.ToSharedRef();
}
void UReticle::SynchronizeProperties() {
	Super::SynchronizeProperties();
	auto ReticleCanvas = NewObject<UCanvas>(this, "ReticleCanvas");
	if(!ReticleTexture) ReticleTexture = NewObject<UCanvasRenderTarget2D>(this, "ReticleTexture");
	Reticle->DrawCrosshair(CrossProperties, ReticleCanvas, ReticleTexture);
}
