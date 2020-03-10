// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "WeaponActualMaster.h"
#include "WeaponPickupMaster.generated.h"

UCLASS()
class MULTIPLAYERSHOOTER_API AWeaponPickupMaster : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeaponPickupMaster();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/** PROPERTIES **/

public:
	//
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Setup", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* BoxCollision;
	//
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Setup", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* WeaponMesh;
	//
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	//class AWeaponActualMaster* WeaponActual = &AWeaponActualMaster();
	TSubclassOf<AWeaponActualMaster> WeaponActual;

};
