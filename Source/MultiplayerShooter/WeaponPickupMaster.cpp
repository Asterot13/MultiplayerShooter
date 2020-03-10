// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponPickupMaster.h"

// Sets default values
AWeaponPickupMaster::AWeaponPickupMaster()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	RootComponent = BoxCollision;

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(BoxCollision);

	bReplicates = true;
}

// Called when the game starts or when spawned
void AWeaponPickupMaster::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AWeaponPickupMaster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

