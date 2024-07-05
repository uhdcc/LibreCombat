// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"

UDamageComponent::UDamageComponent() {
	PrimaryComponentTick.bCanEverTick = true;

	InitialHealth = 100.f;
	Health = 0.f;
	DeathEffect = nullptr;
	RespawnTimer = FTimerHandle();
}
void UDamageComponent::ReceiveDamage(float DamageAmount) {
	Health -= DamageAmount;
	if (Health <= 0.f) {
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, DeathEffect, GetOwner()->GetActorLocation());
		GetOwner()->SetActorEnableCollision(false);
		GetOwner()->SetActorTickEnabled(false);
		GetOwner()->SetActorHiddenInGame(true);
		if (auto CharacterMovement = GetOwner()->FindComponentByClass<UCharacterMovementComponent>()) {
			CharacterMovement->StopActiveMovement();
			CharacterMovement->StopMovementImmediately();
			CharacterMovement->DisableMovement();
		}
		if (auto World = GetWorld()) {
			World->GetTimerManager().SetTimer(
				RespawnTimer,
				this, &UDamageComponent::Respawn,
				2.f,
				false
			);
		}
	}
}
void UDamageComponent::Respawn() {
	Health = InitialHealth;
	GetOwner()->SetActorEnableCollision(true);
	GetOwner()->SetActorTickEnabled(true);
	GetOwner()->SetActorHiddenInGame(false);
	GetOwner()->SetActorLocationAndRotation(FVector::ZeroVector, FRotator::ZeroRotator);
	if (auto CharacterMovement = GetOwner()->FindComponentByClass<UCharacterMovementComponent>()) {
		CharacterMovement->SetMovementMode(EMovementMode::MOVE_Walking);
	}
}
void UDamageComponent::BeginPlay() {
	Super::BeginPlay();
	Health = InitialHealth;
	if (!DeathEffect) {
		DeathEffect = LoadObject<UNiagaraSystem>(
			nullptr,			
			TEXT("NiagaraSystem'/Game/GenericDeathEffect.GenericDeathEffect'")
		);
	}
}
void UDamageComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

