// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponFunctionLibrary.h"
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
#include "WeaponHolder.h"
#include "HUD2.h"

#include "DrawDebugHelpers.h"
AWeapon* UWeaponFunctionLibrary::GiveWeapon(TSubclassOf<AWeapon> WeaponClass, AActor* WeaponOwner) {
	if (!WeaponClass || !WeaponOwner) {
		return nullptr;
	}
	if (auto World = WeaponOwner->GetWorld()) {
		// spawn
		FActorSpawnParameters WeaponSpawnParams;
		WeaponSpawnParams.Owner = WeaponOwner;
		WeaponSpawnParams.Instigator = Cast<APawn>(WeaponOwner);
		auto NewWeapon = World->SpawnActor<AWeapon>(WeaponClass, WeaponSpawnParams);
		// if owner has a controller...
		if (WeaponOwner->GetInstigatorController()) {
			if (auto PlayerController = Cast<APlayerController>(WeaponOwner->GetInstigatorController())) {
				// inputs
				if (WeaponOwner->InputComponent) {
					NewWeapon->InputComponent = WeaponOwner->InputComponent;
					NewWeapon->GetComponents<UButtonComponent>(NewWeapon->Buttons);
					for (auto i : NewWeapon->Buttons) {
						i->BindInput(*NewWeapon->InputComponent);
					}
				}
				// attach
				auto PCM = PlayerController->PlayerCameraManager->GetRootComponent();
				NewWeapon->AttachToComponent(PCM, FAttachmentTransformRules::KeepRelativeTransform);
				// hud
				if (auto Hud = Cast<AHUD2>(PlayerController->GetHUD())) {
					auto NewName = NewWeapon->GetFName().ToString();
					NewName = NewName.Left(NewName.Find("_"));
					NewWeapon->HudParameters.WeaponName = FText::FromString(NewName);
					if (!NewWeapon->HudParameters.ReticleTexture) {
						NewWeapon->HudParameters.ReticleTexture = LoadObject<UTexture2D>(
							nullptr,
							TEXT("Texture2D'/Game/Weapons/CircleReticle.CircleReticle'")
						);
					}
					Hud->AddWeaponToList(NewWeapon->HudParameters);
					NewWeapon->Hud2 = Hud;
				}
			}
		}
		else {
			NewWeapon->AttachToActor(WeaponOwner, FAttachmentTransformRules::KeepRelativeTransform);
		}
		// post spawn
		WeaponOwner->GetAttachedActors(NewWeapon->IgnoredActors);
		NewWeapon->IgnoredActors.Add(WeaponOwner);
		NewWeapon->Unequip();
		if (auto WeaponHolder = WeaponOwner->FindComponentByClass<UWeaponHolder>()) {
			auto Index = WeaponHolder->AddWeapon(NewWeapon);
			WeaponHolder->SelectWeapon(Index);
		}
		return NewWeapon;
	}
	return nullptr;
}
void UWeaponFunctionLibrary::TakeWeapon(AWeapon* Weapon) {
	Weapon->Unequip();
	Weapon->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
	if (Weapon->Hud2) {
		Weapon->Hud2->RemoveWeaponFromList(Weapon->HudParameters);
	}
	if (Weapon->GetOwner()) {
		if (auto WeaponHolder = Weapon->GetOwner()->FindComponentByClass<UWeaponHolder>()) {
			WeaponHolder->Weapons.Remove(Weapon);
			WeaponHolder->CurrentWeaponIndex = 0;
			WeaponHolder->SelectWeapon(0);
		}
	}
}
FHitResult UWeaponFunctionLibrary::HitscanInternal(const FHitscanParameters& HitParameters, const FVector& Start, const FVector& End, float Radius) {
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
FHitResult UWeaponFunctionLibrary::CurvedHitscan(const FCurvedHitscanParameters& HitParameters) {
	if (HitParameters.Angle == 1.f) return Hitscan(HitParameters);
	if (auto World = HitParameters.Owner->GetWorld()) {
		FHitResult Output;
		FVector RayOrigin = HitParameters.Owner->GetActorLocation();
		FVector RayDirection = HitParameters.Owner->GetActorForwardVector();
		const FVector ShotEndPoint = RayOrigin + (RayDirection * 100000.f);
		// early-out if we're aiming directly at a something
		Output = HitscanInternal(HitParameters, RayOrigin, ShotEndPoint);
		//if (Output.GetComponent() && Output.GetComponent()->GetCollisionObjectType() != ECollisionChannel::ECC_WorldStatic) {
		//	return Output;
		//}
		FCollisionQueryParams CollisionQueryParams;
		CollisionQueryParams.AddIgnoredActors(HitParameters.IgnoredActors);
		TArray<FHitResult> Hits;
		// giant hitscan gets everything near reticle
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
				if(Hit.GetComponent()->GetCollisionObjectType() != ECollisionChannel::ECC_WorldStatic){
					TArray<FHitResult> Hits2;
					// sweep a box towards each impact
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
						// find best hit
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
void UWeaponFunctionLibrary::CurvedHitscanWeaponSequence(const FCurvedHitscanParameters& HitParameters, const FWeaponSequence& Sequence) {
	auto Hit = CurvedHitscan(HitParameters);
	HandleHitscan(Hit, HitParameters);
	FinishWeaponSequence(Sequence);
}
FHitResult UWeaponFunctionLibrary::Hitscan(const FHitscanParameters& HitParameters) {
	const FVector ShotEndPoint = HitParameters.Owner->GetActorLocation() + (HitParameters.Owner->GetActorForwardVector() * 100000.f);
	auto Hit = HitscanInternal(HitParameters, HitParameters.Owner->GetActorLocation(), ShotEndPoint);
	return Hit;
}
void UWeaponFunctionLibrary::HandleHitscan(const FHitResult& Hit, const FHitscanParameters& HitParameters) {
	if (!HitParameters.Owner) return;
	if (Hit.GetActor()) {
		if (auto VictimDamageComponent = Hit.GetActor()->FindComponentByClass<UDamageComponent>()) {
			FDamageParameters DamageParams;
			DamageParams.Damage = HitParameters.Damage;
			DamageParams.bBleedthrough = true;
			DamageParams.bWasHeadshot = (Hit.BoneName.ToString().Contains("head"));
			VictimDamageComponent->ReceiveDamage(DamageParams);
			UGameplayStatics::PlaySound2D(HitParameters.Owner, HitParameters.HitSound);
		}
	}
	if (Hit.GetComponent()) {
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
		if (Hit.GetComponent()->IsSimulatingPhysics(Hit.BoneName)) {
			Hit.GetComponent()->AddImpulseAtLocation(
				(Hit.ImpactPoint - HitParameters.Owner->GetActorLocation()).GetSafeNormal2D() * HitParameters.PhysicsForce,
				Hit.ImpactPoint,
				Hit.BoneName
			);
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

}
void UWeaponFunctionLibrary::HitscanWeaponSequence(const FHitscanParameters& HitParameters, const FWeaponSequence& Sequence) {
	auto Hit = Hitscan(HitParameters);
	HandleHitscan(Hit, HitParameters);
	FinishWeaponSequence(Sequence);
}
void UWeaponFunctionLibrary::ZoomIn(AWeapon* Weapon, float NewFov, float NewSensitivity) {
	if (Weapon) {
		if (auto PlayerController = Weapon->GetInstigatorController<APlayerController2>()) {
			PlayerController->Zoom(NewFov, NewSensitivity);
			Weapon->SetActorHiddenInGame(true);
		}
	}
}
void UWeaponFunctionLibrary::ZoomOut(AWeapon* Weapon) {
	if (Weapon) {
		if (auto PlayerController = Weapon->GetInstigatorController<APlayerController2>()) {
			PlayerController->UnZoom();
			Weapon->SetActorHiddenInGame(false);
		}
	}
}
void UWeaponFunctionLibrary::LaunchProjectile(const FProjectileSpawnParameters& Parameters) {
	if (Parameters.ProjectileOwner) {
		if (auto World = Parameters.ProjectileOwner->GetWorld()) {
			FTransform LaunchTransform;
			auto NewActor = World->SpawnActorDeferred<AActor>(
				Parameters.ProjectileClass,
				FTransform(),
				Parameters.ProjectileOwner,
				Parameters.ProjectileOwner->GetInstigator(),
				ESpawnActorCollisionHandlingMethod::AlwaysSpawn
			);
			if (auto NewProjectile = Cast<AProjectile>(NewActor)) {
				NewProjectile->HitSound = Parameters.HitSound;
			}
			FVector Location;
			FRotator Rotation;
			Parameters.ProjectileOwner->GetActorEyesViewPoint(Location, Rotation);
			NewActor->FinishSpawning(FTransform(Rotation, Location, Parameters.ProjectileOwner->GetActorScale3D()));
		}
	}
}
void UWeaponFunctionLibrary::LaunchProjectileWeaponSequence(const FProjectileSpawnParameters& Parameters, const FWeaponSequence& WeaponSequence) {
	LaunchProjectile(Parameters);
	FinishWeaponSequence(WeaponSequence);
}
void UWeaponFunctionLibrary::FinishWeaponSequence(const FWeaponSequence& WeaponSequence) {
	if (WeaponSequence.Weapon) {
		UGameplayStatics::PlaySoundAtLocation(
			WeaponSequence.Weapon,
			WeaponSequence.WeaponSound,
			WeaponSequence.Weapon->GetActorLocation()
		);
		WeaponSequence.Weapon->FirstPersonMesh->PlayAnimation(WeaponSequence.Animation, false);
	}
}
FHitResult UWeaponFunctionLibrary::HitscanShotgun(const FHitscanShotgunParameters& HitParameters) {
	auto Start = HitParameters.Owner->GetActorLocation();
	auto ForwardQuat = HitParameters.Owner->GetActorForwardVector().ToOrientationQuat();
	auto End = Start + (ForwardQuat.Vector() * 100000.f);
	// todo optimize this, It's calculating PelletsPerRing / 2 multiple times, also maybe we can just use a pre-set array of quaternions
	// and just multiply each one by ForwardQuat
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
void UWeaponFunctionLibrary::HitscanShotgunWeaponSequence(const FHitscanShotgunParameters& HitParameters, const FWeaponSequence& Sequence) {
	auto Hit = HitscanShotgun(HitParameters);
	FinishWeaponSequence(Sequence);
}
