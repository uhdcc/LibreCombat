// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Components/Widget.h"
#include "SReticle.generated.h"

class UCanvasRenderTarget2D;

USTRUCT(BlueprintType)
struct FCrossProperties {
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CrossLength;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CrossGap;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CrossThickness;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor CrossColor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float OutlineThickness;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor OutlineColor;

	FCrossProperties() {
		CrossLength = 6.f;
		CrossGap = 4.f;
		CrossThickness = 2.f;
		CrossColor = FLinearColor::White;
		OutlineThickness = 1.f;
		OutlineColor = FLinearColor::Black;
	}
};
class LIBRECOMBAT_API SReticleWidget : public SCompoundWidget {
public:
	SLATE_BEGIN_ARGS(SReticleWidget) {}
	SLATE_END_ARGS()
	void Construct(const FArguments& InArgs);
	TSharedPtr<SImage> ReticleImage;
	TSharedPtr<FSlateBrush> ReticleSlateBrush;
	void DrawCrosshair(FCrossProperties CrossProperties, UCanvas* ReticleCanvas, UCanvasRenderTarget2D* ReticleTexture);
	void DrawLines(float Midpoint, FCrossProperties CrossProperties, float Thickness, UCanvas* ReticleCanvas, FLinearColor Color);
};
UCLASS()
class LIBRECOMBAT_API UReticle : public UWidget {
	GENERATED_BODY()
public:
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void SynchronizeProperties() override;

		
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCanvasRenderTarget2D* ReticleTexture;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FCrossProperties CrossProperties;

	TSharedPtr<SReticleWidget> Reticle;

};
