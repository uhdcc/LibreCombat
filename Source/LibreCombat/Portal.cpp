// Fill out your copyright notice in the Description page of Project Settings.


#include "Portal.h"
#include "Components/BoxComponent.h"
#include "Components/ArrowComponent.h"

APortal::APortal() {
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	Arrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	Arrow->SetupAttachment(RootComponent);

	Collision = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
	Collision->SetupAttachment(RootComponent);
	Collision->SetCollisionProfileName("OverlapAllDynamic");
	Collision->OnComponentBeginOverlap.AddDynamic(this, &APortal::OnOverlap);

	Model = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Model"));
	Model->SetupAttachment(RootComponent);
	Model->SetCollisionProfileName("NoCollision");

	bSomethingIsPorting = false;
	PortingActors = TArray<AActor*>();
	ExitLocation = FVector::ZeroVector;
}
void APortal::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	if (OtherActor) {
		PortingActors.Add(OtherActor);
		bSomethingIsPorting = true;
	}
}
void APortal::BeginPlay() {
	Super::BeginPlay();
	if (PortalExit) {
		ExitLocation = PortalExit->GetActorLocation();
	}
}
void APortal::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	if (bSomethingIsPorting) {
		for (auto& i : PortingActors) {
			float OffsetZ = i->GetActorLocation().Z - GetActorLocation().Z;
			i->SetActorLocation(ExitLocation + FVector(0.f, 0.f, OffsetZ));
		}
		PortingActors.Empty();
		bSomethingIsPorting = false;
	}
}

