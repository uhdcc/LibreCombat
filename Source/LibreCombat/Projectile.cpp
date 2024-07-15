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
	ProjectileCollision = CreateDefaultSubobject<USphereComponent>(TEXT("ProjectileCollision"));
	RootComponent = ProjectileCollision;
	ProjectileCollision->SetCollisionProfileName("BlockAllDynamic");

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
	if (!Movement->bShouldBounce) {
		Explode();
	}
	else if (auto World = GetWorld()) {
		World->GetTimerManager().SetTimer(
			*new FTimerHandle(),
			this, &AProjectile::Explode,
			0.5f,
			false
		);
	}
}
void AProjectile::Explode() {
	DrawDebugSphere(GetWorld(), GetActorLocation(), ExplosionRadius, 12, FColor::Red, false, 0.5f, 0U, 1.f);
	TArray<FOverlapResult> Overlaps;
	GetWorld()->OverlapMultiByChannel(
		Overlaps,
		GetActorLocation(),
		FQuat::Identity,
		ECC_GameTraceChannel1,
		FCollisionShape::MakeSphere(ExplosionRadius)
	);
	for (auto& i : Overlaps) {
		auto NormalizedDistance = (i.Component->GetComponentLocation() - GetActorLocation()).Size() / ExplosionRadius;
		NormalizedDistance = fminf(NormalizedDistance, 1.f);
		NormalizedDistance = (1.f - NormalizedDistance);
		auto Impulse = ((i.Component->GetComponentLocation() - GetActorLocation()).GetSafeNormal()) * ExplosionForce * NormalizedDistance;
		if (i.Actor.IsValid()) {
			if (GetOwner() && i.GetActor() != GetOwner()->GetOwner() && i.Actor != GetOwner()) {
				if (auto VictimDamageComponent = i.Actor->FindComponentByClass<UDamageComponent>()) {
					FDamageParameters DamageParams;
					DamageParams.Damage = ExplosionDamage * NormalizedDistance;
					DamageParams.bBleedthrough = true;
					VictimDamageComponent->ReceiveDamage(DamageParams);
					UGameplayStatics::PlaySound2D(this, HitSound);
				}
			}
			if (auto CharacterMovement = i.Actor->FindComponentByClass<UCharacterMovementComponent>()) {
				FVector Eyes;
				FRotator fefe;
				i.Actor->GetActorEyesViewPoint(Eyes, fefe);
				Impulse = ((Eyes - GetActorLocation()).GetSafeNormal()) * ExplosionForce * NormalizedDistance;
				CharacterMovement->AddImpulse(Impulse);
			}
		}
		if (i.GetComponent()->IsSimulatingPhysics()) {
			i.GetComponent()->AddImpulseAtLocation(
				Impulse,
				i.GetComponent()->GetComponentLocation()
			);
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
	Destroy();
}
void AProjectile::BeginPlay() {
	Super::BeginPlay();
	
	if (GetOwner()) {
		ProjectileCollision->IgnoreActorWhenMoving(GetOwner(), true);
		TArray < UPrimitiveComponent*> OwnerPrimitives;
		GetOwner()->GetComponents<UPrimitiveComponent>(OwnerPrimitives);
		if (GetOwner()->GetOwner()) {
			ProjectileCollision->IgnoreActorWhenMoving(GetOwner()->GetOwner(), true);
			TArray < UPrimitiveComponent*> OwnerPrimitives2;
			GetOwner()->GetOwner()->GetComponents<UPrimitiveComponent>(OwnerPrimitives2);
			OwnerPrimitives.Append(OwnerPrimitives2);
		}
		for (auto& i : OwnerPrimitives) {
			i->IgnoreActorWhenMoving(this, true);
		}
	}
	SetActorHiddenInGame(true);
	if (auto World = GetWorld()) {
		World->GetTimerManager().SetTimer(
			*new FTimerHandle(),
			[=]() {
				SetActorHiddenInGame(false);
			},
			Movement->InitialSpeed / 3000.f * 0.05f,
			false
			);
	}
}

