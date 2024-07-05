// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Portal.generated.h"

class UBoxComponent;
class UArrowComponent;

UCLASS()
class LIBRECOMBAT_API APortal : public AActor
{
	GENERATED_BODY()
	
public:	
	APortal();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* Collision;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* Model;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UArrowComponent* Arrow;

	UFUNCTION()
	void OnOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	bool bSomethingIsPorting;
	TArray<AActor*> PortingActors;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* PortalExit;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector ExitLocation;

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

};
