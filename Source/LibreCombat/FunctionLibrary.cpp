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
#include "Components/MeshComponent.h"

#include "DrawDebugHelpers.h"

FHitResult UFunctionLibrary::HitscanInternal(const FHitscanParameters& HitParameters, const FVector& Start, const FVector& End, float Radius) {
	if (auto World = HitParameters.Owner->GetWorld()) {
		FHitResult Hit;
		FCollisionQueryParams CollisionQueryParams;
		CollisionQueryParams.AddIgnoredActors(HitParameters.IgnoredActors);		
		World->SweepSingleByChannel(
			Hit,
			Start,
			End,
			FQuat::Identity,
			ECC_Visibility,
			FCollisionShape::MakeSphere(Radius),
			CollisionQueryParams
		);
		return Hit;
	}
	return FHitResult();
}
FHitResult UFunctionLibrary::CurvedHitscan(const FCurvedHitscanParameters& HitParameters) {
	if (auto World = HitParameters.Owner->GetWorld()) {
		FHitResult Output;
		FVector RayOrigin = HitParameters.Owner->GetActorLocation();
		FVector RayDirection = HitParameters.Owner->GetActorForwardVector();
		const FVector ShotEndPoint = RayOrigin + (RayDirection * 100000.f);
		Output = HitscanInternal(HitParameters, RayOrigin, ShotEndPoint);
		if (Output.GetComponent() && Cast<UMeshComponent>(Output.GetComponent())) {
			return Output;
		}
		FCollisionQueryParams CollisionQueryParams;
		CollisionQueryParams.AddIgnoredActors(HitParameters.IgnoredActors);
		TArray<FHitResult> Hits;

		World->SweepMultiByChannel(
			Hits,
			RayOrigin,
			ShotEndPoint,
			FQuat::Identity,
			ECC_GameTraceChannel1,
			FCollisionShape::MakeSphere(200.f),
			CollisionQueryParams
		);
		if (Hits.IsValidIndex(0)) {
			float TestingAngle = 0.f;
			float BiggestAngle = 0.f;
			for (auto& Hit : Hits) {
				if(Cast<UMeshComponent>(Hit.GetComponent())){
					TArray<FHitResult> Hits2;
					World->SweepMultiByChannel(
						Hits2,
						RayOrigin + (RayDirection * FVector::Distance(Hit.ImpactPoint, RayOrigin)),
						Hit.ImpactPoint,
						RayDirection.ToOrientationQuat(),
						ECC_GameTraceChannel1,
						FCollisionShape::MakeBox(FVector(100.f, 1.f, 1.f)),
						CollisionQueryParams
					);
					for (auto& Hit2 : Hits2) {
						if (Hit2.GetComponent() == Hit.GetComponent()) {
							auto WallCheck = HitscanInternal(
								HitParameters,
								RayOrigin,
								RayOrigin + (Hit2.ImpactPoint - RayOrigin) * 1.1f
							);
							if (WallCheck.GetComponent() == Hit2.GetComponent()) {
								TestingAngle = FVector::DotProduct(
									RayDirection,
									(Hit2.ImpactPoint - RayOrigin).GetSafeNormal()
								);
								if (TestingAngle > HitParameters.Angle) {
									if (TestingAngle > BiggestAngle) {
										BiggestAngle = TestingAngle;
										Output = WallCheck;
									}
								}
							}
							break;
						}
					}
				}
			}
		}
		return Output;
	}
	return FHitResult();
}
void UFunctionLibrary::CurvedHitscanWeaponSequence(const FCurvedHitscanParameters& HitParameters, const FWeaponSequence& Sequence) {
	auto Hit = CurvedHitscan(HitParameters);
	HandleHitscan(Hit, HitParameters);
	FinishWeaponSequence(Sequence);
}
FHitResult UFunctionLibrary::Hitscan(const FHitscanParameters& HitParameters) {
	const FVector ShotEndPoint = HitParameters.Owner->GetActorLocation() + (HitParameters.Owner->GetActorForwardVector() * 100000.f);
	auto Hit = HitscanInternal(HitParameters, HitParameters.Owner->GetActorLocation(), ShotEndPoint);
	if (Hit.IsValidBlockingHit()) {
		HandleHitscan(Hit, HitParameters);
	}
	return Hit;
}
void UFunctionLibrary::HandleHitscan(const FHitResult& Hit, const FHitscanParameters& HitParameters) {
	if (!HitParameters.Owner) return;
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, LexToString(Hit.ImpactPoint.ToString()));
	//DrawDebugPoint(HitParameters.Owner->GetWorld(), Hit.ImpactPoint, 20.f, FColor::Red, false, 10.f);
	if (Hit.GetComponent() && Hit.GetComponent()->IsSimulatingPhysics(Hit.BoneName)) {
		Hit.GetComponent()->AddImpulseAtLocation(
			(Hit.ImpactPoint - HitParameters.Owner->GetActorLocation()).GetSafeNormal2D() * HitParameters.PhysicsForce,
			Hit.ImpactPoint,
			Hit.BoneName
		);
	}
	if (Hit.GetActor()) {
		if (Hit.GetActor()->CanBeDamaged()) {
			Hit.GetActor()->TakeDamage(10.f, FDamageEvent(), HitParameters.Owner->GetInstigatorController(), HitParameters.Owner);
		}
		if (auto VictimDamageComponent = Hit.GetActor()->FindComponentByClass<UDamageComponent>()) {
			VictimDamageComponent->ReceiveDamage(HitParameters.Damage);
			UGameplayStatics::PlaySound2D(HitParameters.Owner, HitParameters.HitSound);
		}
	}
	FVector BeamStart;
	if (auto OwningWeapon = Cast<AWeapon>(HitParameters.Owner)) {
		BeamStart = OwningWeapon->FirstPersonMesh->GetSocketLocation("Muzzle");
	}
	else {
		BeamStart = HitParameters.Owner->GetActorLocation();
	}
	auto NewBeam = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		HitParameters.Owner,
		HitParameters.BulletTrail,
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
	UDecalComponent* DecalComp = NewObject<UDecalComponent>(HitParameters.Owner->GetWorld());
	DecalComp->bAllowAnyoneToDestroyMe = true;
	DecalComp->SetDecalMaterial(HitParameters.BulletDecal);
	DecalComp->DecalSize = FVector(3.f, 9.f, 9.f);
	DecalComp->SetUsingAbsoluteScale(true);
	DecalComp->AttachToComponent(Hit.GetComponent(), FAttachmentTransformRules::KeepRelativeTransform, NAME_None);
	auto DecalRotation = Hit.ImpactNormal.ToOrientationRotator();
	DecalRotation.Roll = FMath::FRandRange(0.f, 360.f);
	DecalComp->SetWorldLocationAndRotation(Hit.ImpactPoint - (Hit.ImpactNormal * 1.f), DecalRotation);
	DecalComp->FadeScreenSize = 0.f;
	DecalComp->RegisterComponentWithWorld(HitParameters.Owner->GetWorld());
	DecalComp->SetLifeSpan(5.f);
}
void UFunctionLibrary::HitscanWeaponSequence(const FHitscanParameters& HitParameters, const FWeaponSequence& Sequence) {
	auto Hit = Hitscan(HitParameters);
	HandleHitscan(Hit, HitParameters);
	FinishWeaponSequence(Sequence);
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
	LaunchProjectile(Parameters, Parameters.HitSound);
	FinishWeaponSequence(WeaponSequence);
}
void UFunctionLibrary::FinishWeaponSequence(const FWeaponSequence& WeaponSequence) {
	if (WeaponSequence.Weapon) {
		UGameplayStatics::PlaySoundAtLocation(
			WeaponSequence.Weapon,
			WeaponSequence.WeaponSound,
			WeaponSequence.Weapon->GetActorLocation()
		);
		WeaponSequence.Weapon->FirstPersonMesh->PlayAnimation(WeaponSequence.Animation, false);
	}
}
FHitResult UFunctionLibrary::HitscanShotgun(const FHitscanShotgunParameters& HitParameters) {
	auto Start = HitParameters.Owner->GetActorLocation();
	auto ForwardQuat = HitParameters.Owner->GetActorForwardVector().ToOrientationQuat();
	auto End = Start + (ForwardQuat.Vector() * 100000.f);
	// todo optimize this, It's calculating PelletsPerRing / 2 multiple times, also maybe we can just use a pre-set array of Quats
	// and just multiplay each one by forwardquat
	float y;
	float z;
	int CurrentRing = 1;
	int PelletsPerRing = HitParameters.Pattern.NumberOfPellets / HitParameters.Pattern.NumberOfRings;
	float AngleStep;
	auto Hit = HitscanInternal(HitParameters, Start, End);
	HandleHitscan(Hit, HitParameters);
	while (CurrentRing <= HitParameters.Pattern.NumberOfRings) {
		AngleStep = HitParameters.Pattern.Angle * CurrentRing;
		for (size_t i = 1; i <= PelletsPerRing; i++) {
			float idk = (PI / (PelletsPerRing / 2)) * i + (PI / (CurrentRing * (PelletsPerRing / 2)));
			y = sinf(idk) * AngleStep;
			z = cosf(idk) * AngleStep;
			auto NewQuat = ForwardQuat * FQuat::MakeFromEuler(FVector(0.f, y, z));
			End = Start + (NewQuat.Vector() * 100000.f);
			Hit = HitscanInternal(HitParameters, Start, End);
			HandleHitscan(Hit, HitParameters);
		}
		CurrentRing++;
	}
	return FHitResult();
}
void UFunctionLibrary::HitscanShotgunWeaponSequence(const FHitscanShotgunParameters& HitParameters, const FWeaponSequence& Sequence) {
	auto Hit = HitscanShotgun(HitParameters);
	FinishWeaponSequence(Sequence);
}
