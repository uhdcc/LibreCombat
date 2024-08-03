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

	Eyes = CreateDefaultSubobject<USceneComponent>(TEXT("Eyes"));
	Eyes->SetupAttachment(RootComponent);

	//FirstPersonMesh = CreateOptionalDefaultSubobject<USkeletalMeshComponent>(TEXT("FirstPersonMesh"));
	//FirstPersonMesh->AlwaysLoadOnClient = true;
	//FirstPersonMesh->AlwaysLoadOnServer = true;
	//FirstPersonMesh->bOnlyOwnerSee = true;
	//FirstPersonMesh->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPose;
	//FirstPersonMesh->bCastDynamicShadow = false;
	//FirstPersonMesh->bAffectDynamicIndirectLighting = false;
	//FirstPersonMesh->PrimaryComponentTick.TickGroup = TG_PrePhysics;
	//FirstPersonMesh->SetupAttachment(Eyes);
	//FirstPersonMesh->SetCollisionProfileName("NoCollision");
	//FirstPersonMesh->SetGenerateOverlapEvents(false);
	//FirstPersonMesh->SetCanEverAffectNavigation(false);

	DamageComponent = CreateDefaultSubobject<UDamageComponent>(TEXT("DamageComponent"));
	WeaponHolder = CreateDefaultSubobject<UWeaponHolder>(TEXT("WeaponHolder"));

	bDontMove = false;
	bUseControlRotation = true;
}
void ACharacter2::BeginPlay() {
	Super::BeginPlay();
	if (!bDontMove && GetController()) {
		if (Cast<AAIController>(GetController())) {
			bUseControlRotation = false;
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
	Eyes->SetRelativeLocation(FVector(0.f, 0.f, CharacterMovement2->CurrentCapsuleHeight - 20.f));
	BaseEyeHeight = CharacterMovement2->CurrentCapsuleHeight - 20.f;
}
void ACharacter2::CalcCamera(float DeltaTime, FMinimalViewInfo& OutResult) {
	//Super::CalcCamera(DeltaTime, OutResult);
	OutResult.Location = Eyes->GetComponentLocation();
	OutResult.Rotation = Eyes->GetComponentRotation();
}
void ACharacter2::FaceRotation(FRotator NewControlRotation, float DeltaTime) {
	//Super::FaceRotation(NewControlRotation, DeltaTime);

	if (bUseControlRotation) {
		SetActorRotation(FRotator(0.f, NewControlRotation.Yaw, 0.f));
		Eyes->SetRelativeRotation(FRotator(NewControlRotation.Pitch, 0.f, 0.f));
	}
}
void ACharacter2::Ragdoll() {
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("NoCollision"));
	GetMesh()->SetCollisionProfileName(TEXT("PhysicsActor"));
	GetMesh()->SetAllBodiesSimulatePhysics(true);
}
void ACharacter2::ReverseRagdoll() {
	GetMesh()->SetAllBodiesSimulatePhysics(false);
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -87.f), FRotator(0.f, 270.f, 0.f));
	GetMesh()->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("Pawn"));
	GetMesh()->SetCollisionProfileName(TEXT("BlockAllDynamic"));
}