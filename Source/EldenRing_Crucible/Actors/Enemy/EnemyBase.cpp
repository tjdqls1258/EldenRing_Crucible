#include "Actors/Enemy/EnemyBase.h"
#include "Components/Combat/CombatComponents.h"
#include "Components/Combat/EquipmentComponent.h"
#include "Components/Stat/HealthComponent.h"

AEnemyBase::AEnemyBase()
{
	PrimaryActorTick.bCanEverTick = true;
	Tags.AddUnique(FName("Enemy"));
}

void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void AEnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (combatComponents == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("combatComponents == null"));
		return;
	}
	if (healthComponents->IsDead()) return;
	combatComponents->TryAttack(EWeaponSlot::Right);

}

void AEnemyBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

