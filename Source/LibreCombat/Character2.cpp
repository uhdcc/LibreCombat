// Fill out your copyright notice in the Description page of Project Settings.


#include "Character2.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"

#include "DamageComponent.h"
#include "WeaponHolder.h"

//todo decouple ai and character
#include "AIController.h"
#include "NavigationSystem.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Blueprint\AIAsyncTaskBlueprintProxy.h"



ACharacter2::ACharacter2(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCharacterMovementComponent2>(ACharacter::CharacterMovementComponentName)) {
	CharacterMovement2 = Cast<UCharacterMovementComponent2>(GetCharacterMovement());
	GetCapsuleComponent()->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_Yes;
	GetMesh()->bOwnerNoSee = true;
	GetMesh()->SetCollisionProfileName("BlockAllDynamic");

	DamageComponent = CreateDefaultSubobject<UDamageComponent>(TEXT("DamageComponent"));
	WeaponHolder = CreateDefaultSubobject<UWeaponHolder>(TEXT("WeaponHolder"));

	bDontMove = false;
}
void ACharacter2::BeginPlay() {
	Super::BeginPlay();
	if (!bDontMove && GetController()) {
		if (Cast<AAIController>(GetController())) {
			MoveToRandomPoint(EPathFollowingResult::Success);
		}
	}
}
void ACharacter2::MoveToRandomPoint(EPathFollowingResult::Type MovementResult) {
	FVector RandomPoint;
	UNavigationSystemV1::K2_GetRandomReachablePointInRadius(
		this,
		GetActorLocation(),
		RandomPoint,
		3000.f
		);
	auto AiTask = UAIBlueprintHelperLibrary::CreateMoveToProxyObject(
		this,
		this,
		RandomPoint
	);
	AiTask->OnFail.AddDynamic(this, &ACharacter2::MoveToRandomPoint);
	AiTask->OnSuccess.AddDynamic(this, &ACharacter2::MoveToRandomPoint);
}
void ACharacter2::RecalculateBaseEyeHeight() {
	BaseEyeHeight = CharacterMovement2->CurrentCapsuleHeight - 20.f;
}
void ACharacter2::Ragdoll() {
	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
	GetMesh()->SetAllBodiesSimulatePhysics(true);
}
void ACharacter2::ReverseRagdoll() {
	GetMesh()->SetAllBodiesSimulatePhysics(false);
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -87.f), FRotator(0.f, 270.f, 0.f));
	GetMesh()->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	GetMesh()->SetCollisionProfileName(TEXT("BlockAllDynamic"));
}