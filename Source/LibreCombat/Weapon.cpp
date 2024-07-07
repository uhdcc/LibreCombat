// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "DamageComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "HUD2.h"

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

	PrimaryFire = CreateDefaultSubobject<UButtonComponent>(TEXT("PrimaryFire"));
	SecondaryFire = CreateDefaultSubobject<UButtonComponent>(TEXT("SecondaryFire"));

	bIsEquipped = false;
}
UButtonComponent::UButtonComponent() {
	CooldownDuration = 0.3f;
	CooldownTimer = FTimerHandle();
	bIsCoolingDown = false;
	bButtonIsHeld = false;
	bIsEquipped = false;
}
void UButtonComponent::BindInput(UInputComponent& InputComponent) {
	InputComponent.BindAction<TDelegate<void(bool)>>(GetFName(), IE_Pressed, this, &UButtonComponent::Action, true);
	InputComponent.BindAction<TDelegate<void(bool)>>(GetFName(), IE_Released, this, &UButtonComponent::Action, false);
}
void UButtonComponent::StartCooldown() {
	bIsCoolingDown = false;
	if (bButtonIsHeld) {
		Action(true);
	}
}
void UButtonComponent::Action(bool bButtonWasPressed) {
	bButtonIsHeld = bButtonWasPressed;
	if (bIsEquipped && !bIsCoolingDown) {
		if (auto World = GetWorld()) {
			if (ButtonEvent.IsBound()) {
				ButtonEvent.Broadcast(this);
			}
			if (CooldownDuration > 0.f) {
				bIsCoolingDown = true;
				World->GetTimerManager().SetTimer(
					CooldownTimer,
					this, &UButtonComponent::StartCooldown,
					CooldownDuration,
					false
				);
			}
		}
	}
}
void AWeapon::BeginPlay() {
	if(GetOwner()) {
		if(auto PawnOwner = Cast<APawn>(GetOwner())) {
			SetInstigator(PawnOwner);
		}
		if(GetOwner()->GetInstigatorController()) {
			if(auto PlayerController = Cast<APlayerController>(GetOwner()->GetInstigatorController())) {
				if(GetOwner()->InputComponent) {
					InputComponent = GetOwner()->InputComponent;
					GetComponents<UButtonComponent>(Buttons);
					for (auto i : Buttons) {
						i->BindInput(*InputComponent);
					}
				}
				auto PCM = PlayerController->PlayerCameraManager->GetRootComponent();
				AttachToComponent(PCM, FAttachmentTransformRules::KeepRelativeTransform);
				if (auto Hud = Cast<AHUD2>(PlayerController->GetHUD())) {
					auto NewName = GetFName().ToString();
					NewName = NewName.Left(NewName.Find("_"));
					HudParameters.WeaponName = FText::FromString(NewName);
					if (!HudParameters.ReticleTexture) {
						HudParameters.ReticleTexture = LoadObject<UTexture2D>(
							nullptr,
							TEXT("Texture2D'/Game/Weapons/CircleReticle.CircleReticle'")
							//Texture2D'/Engine/EditorLandscapeResources/WhiteSquareTexture.WhiteSquareTexture'
						);
					}
					Hud->AddWeaponToList(HudParameters);
					Hud2 = Hud;
				}
			}
		}
		else {
			AttachToActor(GetOwner(), FAttachmentTransformRules::KeepRelativeTransform);
		}
		GetOwner()->GetAttachedActors(IgnoredActors);
		IgnoredActors.Add(GetOwner());
	}
	Unequip();
	Super::BeginPlay();
}

void AWeapon::Equip_Implementation() {
	ThirdPersonMesh->SetVisibility(true);
	FirstPersonMesh->SetVisibility(true);
	bIsEquipped = true;
	for (auto i : Buttons) {
		i->bIsEquipped = true;
		if (i->bButtonIsHeld) {
			i->Action(true);
		}
	}
	if (Hud2) {
		Hud2->OnEquipWeapon(HudParameters);
	}
}
void AWeapon::Unequip_Implementation() {
	ThirdPersonMesh->SetVisibility(false);
	FirstPersonMesh->SetVisibility(false);
	bIsEquipped = false;
	for (auto i : Buttons) {
		i->bIsEquipped = false;
	}
}
