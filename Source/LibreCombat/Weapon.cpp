// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "DrawDebugHelpers.h"
#include "DamageComponent.h"

AWeapon::AWeapon()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	ThirdPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ThirdPersonMesh"));
	ThirdPersonMesh->SetupAttachment(RootComponent);
	ThirdPersonMesh->AlwaysLoadOnClient = true;
	ThirdPersonMesh->AlwaysLoadOnServer = true;
	ThirdPersonMesh->bOwnerNoSee = true;
	ThirdPersonMesh->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPose;
	ThirdPersonMesh->bCastDynamicShadow = true;
	ThirdPersonMesh->bAffectDynamicIndirectLighting = true;
	ThirdPersonMesh->PrimaryComponentTick.TickGroup = TG_PrePhysics;
	ThirdPersonMesh->SetGenerateOverlapEvents(false);
	ThirdPersonMesh->SetCanEverAffectNavigation(false);
	ThirdPersonMesh->SetIsReplicated(true);

	FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FirstPersonMesh"));
	FirstPersonMesh->SetupAttachment(RootComponent);
	FirstPersonMesh->bOnlyOwnerSee = true;
	FirstPersonMesh->CastShadow = false;
	FirstPersonMesh->SetIsReplicated(true);

	bIsEquipped = false;
	PhysicsForce = 10000.f;
	Damage = 20.f;
}

void AWeapon::BeginPlay() {
	Super::BeginPlay();
	if(GetOwner()) {
		if(auto PawnOwner = Cast<APawn>(GetOwner())) {
			SetInstigator(PawnOwner);
		}
		if(GetOwner()->InputComponent) {
			InputComponent = GetOwner()->InputComponent;
			InputComponent->BindAction<TDelegate<void(bool)>>("Shoot", IE_Pressed, this, &AWeapon::Shoot, true);
			InputComponent->BindAction<TDelegate<void(bool)>>("Shoot", IE_Released, this, &AWeapon::Shoot, false);
		}
		if(!GetOwner()->GetInstigatorController()) {
			AttachToActor(GetOwner(), FAttachmentTransformRules::KeepRelativeTransform);
		}
		else if(auto PlayerController = Cast<APlayerController>(GetOwner()->GetInstigatorController())) {
			auto PCM = PlayerController->PlayerCameraManager->GetRootComponent();
			AttachToComponent(PCM, FAttachmentTransformRules::KeepRelativeTransform);
		}
	}
	Unequip();
}

void AWeapon::Shoot(bool bButtonWasPressed) {
	if(bIsEquipped) {
		if(bButtonWasPressed) {
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Pressed");
			if(auto World = GetWorld()) {
				FHitResult Hit;
				const FVector ShotEndPoint = GetActorLocation() + (GetActorForwardVector() * 100000.f);
				World->LineTraceSingleByChannel(Hit, GetActorLocation(), ShotEndPoint, ECC_Visibility);
				DrawDebugLine(
					World, GetActorLocation() - FVector(0.f, 0.f, 10.f), 
					Hit.IsValidBlockingHit() ? Hit.ImpactPoint : Hit.TraceEnd,
					FColor::Red, false, 0.5f, 0U, 2.f);
				if (Hit.IsValidBlockingHit()) {
					DrawDebugPoint(World, Hit.ImpactPoint, 20.f, FColor::Green, false, 0.5f);
					if (Hit.GetComponent()->IsSimulatingPhysics(Hit.BoneName)) {
						Hit.GetComponent()->AddImpulseAtLocation(
							(Hit.ImpactPoint - GetActorLocation()).GetSafeNormal2D() * PhysicsForce,
							Hit.ImpactPoint,
							Hit.BoneName
						);
					}
					if (Hit.GetActor() && Hit.GetActor()->CanBeDamaged()) {
						Hit.GetActor()->TakeDamage(10.f, FDamageEvent(), GetInstigatorController(), this);
					}
					if (Hit.GetActor()) {
						if (auto VictimDamageComponent = Hit.GetActor()->FindComponentByClass<UDamageComponent>()) {
							VictimDamageComponent->ReceiveDamage(Damage);
						}
					}
					else {
						DrawDebugLine(World, GetActorLocation(), ShotEndPoint, FColor::Red, false, 0.5f, 0U, 2.f);
					}
				}
			}
		}
		else {
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Released");
		}
	}
}

void AWeapon::Equip() {
	ThirdPersonMesh->SetVisibility(true);
	FirstPersonMesh->SetVisibility(true);
	bIsEquipped = true;
}
void AWeapon::Unequip() {
	ThirdPersonMesh->SetVisibility(false);
	FirstPersonMesh->SetVisibility(false);
	bIsEquipped = false;
}
