// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DamageComponent.h"

AProjectile::AProjectile() {
	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	RootComponent = Collision;
	Collision->SetCollisionProfileName("BlockAllDynamic");

	Model = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Model"));
	Model->SetupAttachment(RootComponent);
	Model->SetCollisionProfileName("NoCollision");


	Movement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Movement"));
	Movement->OnProjectileBounce.AddDynamic(this, &AProjectile::Bounce);
	Movement->OnProjectileStop.AddDynamic(this, &AProjectile::Stopped);

	InitialLifeSpan = 15.f;
	BounceSound = nullptr;
	ExplosionSound = nullptr;
	ExplosionEffect = nullptr;
	ExplosionRadius = 1000.f;
	ExplosionForce = 1000.f;
	ExplosionDamage = 100.f;
}
void AProjectile::Bounce(const FHitResult& ImpactResult, const FVector& ImpactVelocity) {
	UGameplayStatics::PlaySoundAtLocation(
		this,
		BounceSound,
		GetActorLocation()
	);
}
void AProjectile::Stopped(const FHitResult& ImpactResult) {
	if (auto World = GetWorld()) {
		World->GetTimerManager().SetTimer(
			*new FTimerHandle(),
			this, &AProjectile::Explode,
			0.5f,
			false
		);
	}
}
void AProjectile::Explode() {
	TArray<FOverlapResult> Overlaps;
	GetWorld()->OverlapMultiByObjectType(
		Overlaps,
		GetActorLocation(),
		FQuat::Identity,
		FCollisionObjectQueryParams(FCollisionObjectQueryParams::InitType::AllObjects),
		FCollisionShape::MakeSphere(ExplosionRadius)
	);
	for (auto& i : Overlaps) {
		auto NormalizedDistance = (i.Component->GetComponentLocation() - GetActorLocation()).Size() / ExplosionRadius;
		NormalizedDistance = fminf(NormalizedDistance, 1.f);
		NormalizedDistance = (1.f - NormalizedDistance);
		auto Impulse = ((i.Component->GetComponentLocation() - GetActorLocation()).GetSafeNormal() + FVector(0.f, 0.f, 0.1f)) * ExplosionForce * NormalizedDistance;
		if (i.Component->IsAnySimulatingPhysics()) {
			i.Component->AddImpulse(Impulse);
		}
		if (i.Actor.IsValid()) {
			if (auto CharacterMovement = i.Actor->FindComponentByClass<UCharacterMovementComponent>()) {
				CharacterMovement->AddImpulse(Impulse);
			}
			if (auto VictimDamageComponent = i.Actor->FindComponentByClass<UDamageComponent>()) {
				VictimDamageComponent->ReceiveDamage(ExplosionDamage * NormalizedDistance);
			}
		}
	}

	UGameplayStatics::PlaySoundAtLocation(
		this,
		ExplosionSound,
		GetActorLocation()
	);
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		this, 
		ExplosionEffect, 
		GetActorLocation()
	);
	ConditionalBeginDestroy();
}
void AProjectile::BeginPlay() {
	Super::BeginPlay();
	
	if (GetOwner()) {
		Collision->IgnoreActorWhenMoving(GetOwner(), true);
		TArray < UPrimitiveComponent*> OwnerPrimitives;
		GetOwner()->GetComponents<UPrimitiveComponent>(OwnerPrimitives);

		for (auto& i : OwnerPrimitives) {
			i->IgnoreActorWhenMoving(this, true);
		}
	}
}

