#include "Actors/Enemy/EnemyBase.h"
#include "Components/Combat/CombatComponents.h"
#include "Components/Combat/EquipmentComponent.h"

AEnemyBase::AEnemyBase()
{
	PrimaryActorTick.bCanEverTick = true;
	Tags.AddUnique(FName("Enemy"));

	combatComponents = CreateDefaultSubobject<UCombatComponents>(TEXT("CombatComponts"));
	equipmentComponents = CreateDefaultSubobject<UEquipmentComponent>(TEXT("EquipmentCompoents"));
}

void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void AEnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEnemyBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

