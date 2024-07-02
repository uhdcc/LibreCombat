// Fill out your copyright notice in the Description page of Project Settings.


#include "FunctionLibrary.h"
#include "Weapon.h"
#include "DamageComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerController2.h"
#include "Components/DecalComponent.h"

FHitResult UFunctionLibrary::Hitscan(const FHitscanParameters& HitParameters) {
	if (auto World = HitParameters.Weapon->GetWorld()) {
		FHitResult Hit;
		const FVector ShotEndPoint = HitParameters.Weapon->GetActorLocation() + (HitParameters.Weapon->GetActorForwardVector() * 100000.f);
		World->LineTraceSingleByChannel(Hit, HitParameters.Weapon->GetActorLocation(), ShotEndPoint, ECC_Visibility);

		if (Hit.IsValidBlockingHit()) {
			if (Hit.GetComponent()->IsSimulatingPhysics(Hit.BoneName)) {
				Hit.GetComponent()->AddImpulseAtLocation(
					(Hit.ImpactPoint - HitParameters.Weapon->GetActorLocation()).GetSafeNormal2D() * HitParameters.PhysicsForce,
					Hit.ImpactPoint,
					Hit.BoneName
				);
			}
			if (Hit.GetActor() && Hit.GetActor()->CanBeDamaged()) {
				Hit.GetActor()->TakeDamage(10.f, FDamageEvent(), HitParameters.Weapon->GetInstigatorController(), HitParameters.Weapon);
			}
			if (Hit.GetActor()) {
				if (auto VictimDamageComponent = Hit.GetActor()->FindComponentByClass<UDamageComponent>()) {
					VictimDamageComponent->ReceiveDamage(HitParameters.Damage);
				}
			}
		}
		return Hit;
	}
	return FHitResult();
}

void UFunctionLibrary::HitscanWeaponSequence(const FHitscanParameters& HitParameters, const FHitscanSequenceParameters& Sequence) {
	if (HitParameters.Weapon) {
		auto Hit = Hitscan(HitParameters);
		if (Hit.GetActor()) {
			if (auto VictimDamageComponent = Hit.GetActor()->FindComponentByClass<UDamageComponent>()) {
				UGameplayStatics::PlaySound2D(HitParameters.Weapon, Sequence.HitSound);
			}
		}
		auto BeamStart = HitParameters.Weapon->FirstPersonMesh->GetSocketLocation("Muzzle");
		auto NewBeam = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			HitParameters.Weapon,
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
		UGameplayStatics::PlaySoundAtLocation(
			HitParameters.Weapon,
			Sequence.WeaponSound,
			HitParameters.Weapon->GetActorLocation()
		);
		HitParameters.Weapon->FirstPersonMesh->PlayAnimation(Sequence.Animation, false);


		UDecalComponent* DecalComp = NewObject<UDecalComponent>(HitParameters.Weapon->GetWorld());
		DecalComp->bAllowAnyoneToDestroyMe = true;
		DecalComp->SetDecalMaterial(Sequence.BulletDecal);
		DecalComp->DecalSize = FVector(3.f, 9.f, 9.f);
		DecalComp->SetUsingAbsoluteScale(true);
		DecalComp->SetLifeSpan(5.f);
		DecalComp->FadeScreenSize = 0.f;
		DecalComp->AttachToComponent(Hit.GetComponent(), FAttachmentTransformRules::KeepRelativeTransform, NAME_None);
		DecalComp->SetWorldLocationAndRotation(Hit.ImpactPoint - (Hit.ImpactNormal * 1.f), Hit.ImpactNormal.ToOrientationRotator());
		DecalComp->RegisterComponentWithWorld(HitParameters.Weapon->GetWorld());


		//auto NewDecal = UGameplayStatics::SpawnDecalAttached(
		//	Sequence.BulletDecal,
		//	FVector(3.f, 9.f, 9.f),
		//	Hit.GetComponent(),
		//	NAME_None,
		//	Hit.ImpactPoint - (Hit.ImpactNormal * 1.f),
		//	Hit.ImpactNormal.ToOrientationRotator(),
		//	EAttachLocation::KeepWorldPosition
		//);
		//NewDecal->SetLifeSpan(5.f);
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
