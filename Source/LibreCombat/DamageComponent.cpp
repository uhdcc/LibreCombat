// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/GameStateBase.h"
#include "HUD2.h"
#include "Character2.h"
#include "GameFramework/PlayerStart.h"

#include "EngineUtils.h"
#include "NavMesh/RecastNavMesh.h"

UDamageComponent::UDamageComponent() {
	PrimaryComponentTick.bCanEverTick = true;

	InitialHealth = 120.f;
	Health = 0.f;
	bActorIsDead = false;
	bActorIsImmortal = false;
	bPostDeathHasBeenTicked = false;
	InitialShield = 100.f;
	Shield = 0.f;
	bShieldIsBroken = false;
	bShieldIsRecharging = false;
	RechargeStartingShield = 0.f;
	ElapsedRechargeTime = 0.f;

	DeathEffect = nullptr;
	RespawnTimer = FTimerHandle();
	CurrentRecharge = FRechargeParameters();
}
void UDamageComponent::ReceiveDamage(const FDamageParameters& Parameters) {
	if (bActorIsDead) return;
	float LocalDamage = Parameters.Damage;
	float Difference;
LOOP:
	if (bShieldIsBroken || InitialShield == 0.f) {
		Difference = Health - LocalDamage;
		if (Difference <= 0.f) {
			bActorIsDead = true;
		}
		else if (Parameters.bWasHeadshot) {
			bActorIsDead =  true;
		}
		else {
			SetHealth(Difference);
		}
	}
	else {
		Difference = Shield - LocalDamage;
		if (Difference <= 0.f) {
			if (Parameters.bBleedthrough) {
				if (Parameters.bWasHeadshot) {
					bActorIsDead = true;
				}
				else {
					bShieldIsBroken = true;
					SetShield(0.f);
					LocalDamage = FMath::Abs(Difference);
					goto LOOP;
				}
			}
			else {
				bShieldIsBroken = true;
				SetShield(0.f);
			}
		}
		SetShield(Difference);

		FRechargeParameters RechargeParams;
		RechargeParams.bCanBeInterrupted = true;
		RechargeParams.RechargeDelay = 3.7f;
		RechargeParams.RechargeGoal = InitialShield;
		RechargeParams.RechargeRate = 30.f;
		RechargeParams.RechargeTimestamp = GetTimestamp() + RechargeParams.RechargeDelay;
		StartRecharging(RechargeParams);
	}
	if (bActorIsDead) {
		StopRecharging();
		if (GetOwner()->GetInstigatorController()) GetOwner()->GetInstigatorController()->StopMovement();

		if (auto CharacterMovement = GetOwner()->FindComponentByClass<UCharacterMovementComponent>()) {
			CharacterMovement->StopActiveMovement();
			CharacterMovement->StopMovementImmediately();
			CharacterMovement->DisableMovement();
		}
		if (auto Character2 = Cast<ACharacter2>(GetOwner())) {
			Character2->Ragdoll();
		}
		else {
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, DeathEffect, GetOwner()->GetActorLocation());
			GetOwner()->SetActorEnableCollision(false);
			GetOwner()->SetActorTickEnabled(false);
			GetOwner()->SetActorHiddenInGame(true);
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
	SetHealth(InitialHealth);
	SetShield(InitialShield);
	bShieldIsBroken = false;
	bActorIsDead = false;
	bPostDeathHasBeenTicked = false;
	GetOwner()->SetActorEnableCollision(true);
	GetOwner()->SetActorTickEnabled(true);
	GetOwner()->SetActorHiddenInGame(false);

	FVector SpawnPoint = FVector::ZeroVector;
	float SpawnRotation = 0.f;

	if (auto World = GetWorld()) {
		TArray<APlayerStart*> PlayerStarts;
		for (TActorIterator<APlayerStart> i(World); i; ++i) {
			PlayerStarts.Add(*i);
		}
		if (PlayerStarts.IsValidIndex(0)) {
			auto RandomIndex = FMath::RandRange(0, PlayerStarts.Num() - 1);
			TArray<UPrimitiveComponent*> OverlappingComponents;
			PlayerStarts[RandomIndex]->GetOverlappingComponents(OverlappingComponents);
			SpawnPoint = PlayerStarts[RandomIndex]->GetActorLocation();
			SpawnRotation = PlayerStarts[RandomIndex]->GetActorRotation().Yaw;
		}
		else {
			for (TActorIterator<ARecastNavMesh> i(World); i; ++i) {
				SpawnPoint = i->GetRandomPoint().Location + FVector(0.f, 0.f, 100.f);
				SpawnRotation = FMath::RandRange(0.f, 360.f);
				break;
			}
		}
	}
	GetOwner()->SetActorLocationAndRotation(SpawnPoint, FRotator(0.f, SpawnRotation, 0.f));

	if (auto Character2 = Cast<ACharacter2>(GetOwner())) {
		Character2->ReverseRagdoll();
	}
	if (auto CharacterMovement = GetOwner()->FindComponentByClass<UCharacterMovementComponent>()) {
		CharacterMovement->SetMovementMode(EMovementMode::MOVE_Walking);
	}
}
void UDamageComponent::BeginPlay() {
	Super::BeginPlay();
	if (GetOwner() && GetOwner()->GetInstigatorController()) {
		if (auto PlayerController = Cast<APlayerController>(GetOwner()->GetInstigatorController())) {
			HUD2 = Cast<AHUD2>(PlayerController->GetHUD());
			HUD2->InitialHealth = InitialHealth;
			HUD2->InitialShield = InitialShield;
		}
	}
	SetHealth(InitialHealth);
	SetShield(InitialShield);
}
void UDamageComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	if (bActorIsDead && !bPostDeathHasBeenTicked) {
		StopRecharging();
		if(GetOwner()->GetInstigatorController()) GetOwner()->GetInstigatorController()->StopMovement();

		if (auto CharacterMovement = GetOwner()->FindComponentByClass<UCharacterMovementComponent>()) {
			CharacterMovement->StopActiveMovement();
			CharacterMovement->StopMovementImmediately();
			CharacterMovement->DisableMovement();
		}
		if (auto Character2 = Cast<ACharacter2>(GetOwner())) {
			Character2->Ragdoll();
		}
		else {
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, DeathEffect, GetOwner()->GetActorLocation());
			GetOwner()->SetActorEnableCollision(false);
			GetOwner()->SetActorTickEnabled(false);
			GetOwner()->SetActorHiddenInGame(true);
		}
		if (auto World = GetWorld()) {
			World->GetTimerManager().SetTimer(
				RespawnTimer,
				this, &UDamageComponent::Respawn,
				2.f,
				false
			);
		}
		bPostDeathHasBeenTicked = true;
	}
	else {
		if (bShieldIsRecharging) {
			ElapsedRechargeTime = GetWorld()->GetGameState()->GetServerWorldTimeSeconds() - CurrentRecharge.RechargeTimestamp;
			if (ElapsedRechargeTime > 0) {
				SetShield(RechargeStartingShield + ElapsedRechargeTime * CurrentRecharge.RechargeRate);
				if (Shield > CurrentRecharge.RechargeGoal) {
					StopRecharging();
					SetShield(CurrentRecharge.RechargeGoal);
					bShieldIsBroken = false;
				}
			}
		}
	}
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}
void UDamageComponent::StartRecharging(const FRechargeParameters& Parameters) {
	if (CurrentRecharge.bCanBeInterrupted) {
		if (Shield > Parameters.RechargeGoal) {
			return;
		}
		CurrentRecharge = Parameters;
		RechargeStartingShield = Shield;
		bShieldIsRecharging = true;
	}
}
void UDamageComponent::StopRecharging() {
	bShieldIsRecharging = false;
	CurrentRecharge.bCanBeInterrupted = true;
}
float UDamageComponent::GetTimestamp() {
	if (GetWorld() && GetWorld()->GetGameState()) {
		return GetWorld()->GetGameState()->GetServerWorldTimeSeconds();
	}
	return 0.f;
}
void UDamageComponent::SetHealth(float NewHealth) {
	Health = NewHealth;
	if (HUD2) HUD2->SetHealth(NewHealth);
}
void UDamageComponent::SetShield(float NewShield) {
	Shield = NewShield;
	if (HUD2) HUD2->SetShield(NewShield);
}

float UDamageComponent::GetHealth() {
	return Health;
}

float UDamageComponent::GetShield() {
	return Shield;
}
