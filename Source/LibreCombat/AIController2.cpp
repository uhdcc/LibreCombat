// Fill out your copyright notice in the Description page of Project Settings.


#include "AIController2.h"
#include "GameFramework/CharacterMovementComponent.h"

AAIController2::AAIController2() {
	GetPathFollowingComponent()->SetBlockDetection(10.f, 0.5f, 2);
}

void AAIController2::BeginPlay() {
	if (GetPawn()) {
		GetPawn()->bUseControllerRotationYaw = false;
		if (auto CharacterMovement = GetPawn()->FindComponentByClass<UCharacterMovementComponent>()) {
			CharacterMovement->RotationRate = FRotator(0.f, 250.f, 0.f);
			CharacterMovement->bOrientRotationToMovement = true;
		}
	}

	Super::BeginPlay();
}
