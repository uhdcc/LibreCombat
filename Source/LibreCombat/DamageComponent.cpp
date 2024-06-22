// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageComponent.h"
#include "D:\Apps\Epic Games\UE_4.27\Engine\Plugins\FX\Niagara\Source\Niagara\Public\NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"

// Sets default values for this component's properties
UDamageComponent::UDamageComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...

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
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Test");
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, LexToString(DamageAmount));
}

void UDamageComponent::Respawn()
{
	Health = InitialHealth;
	GetOwner()->SetActorEnableCollision(true);
	GetOwner()->SetActorTickEnabled(true);
	GetOwner()->SetActorHiddenInGame(false);
	GetOwner()->SetActorLocationAndRotation(FVector::ZeroVector, FRotator::ZeroRotator);
}

// Called when the game starts
void UDamageComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
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


// Called every frame
void UDamageComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

