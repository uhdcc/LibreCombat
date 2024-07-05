// Fill out your copyright notice in the Description page of Project Settings.


#include "FunctionLibrary.h"
#include "Weapon.h"
#include "DamageComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerController2.h"
#include "Components/DecalComponent.h"
#include "Projectile.h"
#include "Kismet/KismetSystemLibrary.h"
#include "DrawDebugHelpers.h"

FHitResult UFunctionLibrary::CurvedHitscan(const FHitscanParameters& HitParameters) {
	if (auto World = HitParameters.Owner->GetWorld()) {
		const FVector ShotEndPoint = HitParameters.Owner->GetActorLocation() + (HitParameters.Owner->GetActorForwardVector() * 100000.f);
		FCollisionQueryParams CollisionQueryParams;
		CollisionQueryParams.AddIgnoredActors(HitParameters.IgnoredActors);
		TArray<FHitResult> Hits;

		World->SweepMultiByChannel(
			Hits,
			HitParameters.Owner->GetActorLocation(),
			ShotEndPoint,
			FQuat::Identity,
			ECC_GameTraceChannel1,
			FCollisionShape::MakeSphere(200.f),
			CollisionQueryParams
		);

		if (Hits.IsValidIndex(0)) {
			for (auto& Hit : Hits) {
				

				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, LexToString(
					FVector::DotProduct(
						HitParameters.Owner->GetActorForwardVector(), 
						(Hit.ImpactPoint - HitParameters.Owner->GetActorLocation()).GetSafeNormal()
					)
				));
				HandleHitscan(Hit, HitParameters);
			}
			return Hits[0];
		}
	}
	return FHitResult();
}

FHitResult UFunctionLibrary::Hitscan(const FHitscanParameters& HitParameters) {
	if (auto World = HitParameters.Owner->GetWorld()) {
		FHitResult Hit;
		const FVector ShotEndPoint = HitParameters.Owner->GetActorLocation() + (HitParameters.Owner->GetActorForwardVector() * 100000.f);
		FCollisionQueryParams CollisionQueryParams;
		CollisionQueryParams.AddIgnoredActors(HitParameters.IgnoredActors);
		World->LineTraceSingleByChannel(
			Hit, 
			HitParameters.Owner->GetActorLocation(), 
			ShotEndPoint, 
			ECC_Visibility,
			CollisionQueryParams
		);
		if (Hit.IsValidBlockingHit()) {
			HandleHitscan(Hit, HitParameters);
			return Hit;
		}
	}
	return FHitResult();
}

void UFunctionLibrary::HandleHitscan(const FHitResult& Hit, const FHitscanParameters& HitParameters) {
	DrawDebugPoint(HitParameters.Owner->GetWorld(), Hit.ImpactPoint, 20.f, FColor::Red, false, 10.f);

	//if (Hit.IsValidBlockingHit()) {
		if (Hit.GetComponent()->IsSimulatingPhysics(Hit.BoneName)) {
			Hit.GetComponent()->AddImpulseAtLocation(
				(Hit.ImpactPoint - HitParameters.Owner->GetActorLocation()).GetSafeNormal2D() * HitParameters.PhysicsForce,
				Hit.ImpactPoint,
				Hit.BoneName
			);
		}
		if (Hit.GetActor() && Hit.GetActor()->CanBeDamaged()) {
			Hit.GetActor()->TakeDamage(10.f, FDamageEvent(), HitParameters.Owner->GetInstigatorController(), HitParameters.Owner);
		}
		if (Hit.GetActor()) {
			if (auto VictimDamageComponent = Hit.GetActor()->FindComponentByClass<UDamageComponent>()) {
				VictimDamageComponent->ReceiveDamage(HitParameters.Damage);
			}
		}
	//}
}

void UFunctionLibrary::HitscanWeaponSequence(const FHitscanParameters& HitParameters, const FHitscanSequence& Sequence) {
	if (Sequence.Weapon) {
		auto Hit = Hitscan(HitParameters);
		if (Hit.GetActor()) {
			if (auto VictimDamageComponent = Hit.GetActor()->FindComponentByClass<UDamageComponent>()) {
				UGameplayStatics::PlaySound2D(Sequence.Weapon, Sequence.HitSound);
			}
		}
		auto BeamStart = Sequence.Weapon->FirstPersonMesh->GetSocketLocation("Muzzle");
		auto NewBeam = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			Sequence.Weapon,
			Sequence.BulletTrail,
			BeamStart,
			FRotator::ZeroRotator,
			FVector(1.f),
			true,
			false,
			ENCPoolMethod::AutoRelease
		);
		if (NewBeam) {
			NewBeam->SetVectorParameter("BeamEnd", Hit.IsValidBlockingHit() ? Hit.ImpactPoint : Hit.TraceEnd);
			NewBeam->ActivateSystem();
		}
		UDecalComponent* DecalComp = NewObject<UDecalComponent>(Sequence.Weapon->GetWorld());
		DecalComp->bAllowAnyoneToDestroyMe = true;
		DecalComp->SetDecalMaterial(Sequence.BulletDecal);
		DecalComp->DecalSize = FVector(3.f, 9.f, 9.f);
		DecalComp->SetUsingAbsoluteScale(true);
		DecalComp->SetLifeSpan(5.f);
		DecalComp->FadeScreenSize = 0.f;
		DecalComp->AttachToComponent(Hit.GetComponent(), FAttachmentTransformRules::KeepRelativeTransform, NAME_None);
		auto DecalRotation = Hit.ImpactNormal.ToOrientationRotator();
		DecalRotation.Roll = FMath::FRandRange(0.f, 360.f);
		DecalComp->SetWorldLocationAndRotation(Hit.ImpactPoint - (Hit.ImpactNormal * 1.f), DecalRotation);
		DecalComp->RegisterComponentWithWorld(Sequence.Weapon->GetWorld());

		FinishWeaponSequence(Sequence);
	}
}

void UFunctionLibrary::ZoomIn(AWeapon* Weapon, float NewFov, float NewSensitivity) {
	if (Weapon) {
		if (auto PlayerController = Weapon->GetInstigatorController<APlayerController2>()) {
			PlayerController->Zoom(NewFov, NewSensitivity);
			Weapon->SetActorHiddenInGame(true);
		}
	}
}

void UFunctionLibrary::ZoomOut(AWeapon* Weapon) {
	if (Weapon) {
		if (auto PlayerController = Weapon->GetInstigatorController<APlayerController2>()) {
			PlayerController->UnZoom();
			Weapon->SetActorHiddenInGame(false);
		}
	}
}

void UFunctionLibrary::LaunchProjectile(const FProjectileSpawnParameters& Parameters, USoundBase* HitSound) {
	if (Parameters.Owner) {
		if (auto World = Parameters.Owner->GetWorld()) {			
			auto NewActor = World->SpawnActorDeferred<AActor>(
				Parameters.Class,
				Parameters.Owner->GetActorTransform(),
				Parameters.Owner,
				Parameters.Owner->GetInstigator(),
				ESpawnActorCollisionHandlingMethod::AlwaysSpawn
			);
			if (auto NewProjectile = Cast<AProjectile>(NewActor)) {
				NewProjectile->HitSound = HitSound;
			}
			NewActor->FinishSpawning(Parameters.Owner->GetActorTransform());
		}
	}
}

void UFunctionLibrary::LaunchProjectileWeaponSequence(const FProjectileSpawnParameters& Parameters, const FWeaponSequence& WeaponSequence) {
	LaunchProjectile(Parameters, WeaponSequence.HitSound);
	FinishWeaponSequence(WeaponSequence);
}

void UFunctionLibrary::FinishWeaponSequence(const FWeaponSequence& WeaponSequence) {
	UGameplayStatics::PlaySoundAtLocation(
		WeaponSequence.Weapon,
		WeaponSequence.WeaponSound,
		WeaponSequence.Weapon->GetActorLocation()
	);
	WeaponSequence.Weapon->FirstPersonMesh->PlayAnimation(WeaponSequence.Animation, false);
}
