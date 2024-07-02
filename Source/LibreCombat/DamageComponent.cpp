// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"

UDamageComponent::UDamageComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	InitialHealth = 100.f;
	Health = 0.f;
	DeathEffect = nullptr;
	RespawnTimer = FTimerHandle();
}


void UDamageComponent::OnTakeAnyDamage(
	AActor* DamagedActor, 
	float Damage, 
	const UDamageType* DamageType, 
	AController* InstigatedBy, 
	AActor* DamageCauser){

}

void UDamageComponent::ReceiveDamage(float DamageAmount)
{
	Health -= DamageAmount;
	if (Health <= 0.f) {
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, DeathEffect, GetOwner()->GetActorLocation());
		GetOwner()->SetActorEnableCollision(false);
		GetOwner()->SetActorTickEnabled(false);
		GetOwner()->SetActorHiddenInGame(true);

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

void UDamageComponent::Respawn()
{
	Health = InitialHealth;
	GetOwner()->SetActorEnableCollision(true);
	GetOwner()->SetActorTickEnabled(true);
	GetOwner()->SetActorHiddenInGame(false);
	GetOwner()->SetActorLocationAndRotation(FVector::ZeroVector, FRotator::ZeroRotator);
}

void UDamageComponent::BeginPlay()
{
	Super::BeginPlay();

	Health = InitialHealth;

	if (GetOwner()) {
		GetOwner()->OnTakeAnyDamage.AddDynamic(this, &UDamageComponent::OnTakeAnyDamage);
	}

	if (!DeathEffect) {
		DeathEffect = LoadObject<UNiagaraSystem>(
			nullptr,			
			TEXT("NiagaraSystem'/Game/GenericDeathEffect.GenericDeathEffect'")
		);
	}
}


void UDamageComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

