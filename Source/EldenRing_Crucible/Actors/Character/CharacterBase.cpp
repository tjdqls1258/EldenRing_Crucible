#include "Actors/Character/CharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/Stat/HealthComponent.h"
#include "Components/Combat/CombatComponents.h"
#include "Components/Combat/EquipmentComponent.h"
#include "Components/CapsuleComponent.h"

ACharacterBase::ACharacterBase()
{
	PrimaryActorTick.bCanEverTick = false;

	combatComponents = CreateDefaultSubobject<UCombatComponents>(TEXT("CombatComponents"));
	equipmentComponents = CreateDefaultSubobject<UEquipmentComponent>(TEXT("EquipmentCompoents"));
	healthComponents = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponents"));
}

void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();

	healthComponents->OnDeath.AddDynamic(this, &ThisClass::HandleDeath);
}

void ACharacterBase::HandleDeath(AActor* killer)
{
	if (combatComponents)
	{
		combatComponents->CancelAttack();
	}

	GetCharacterMovement()->DisableMovement();
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	OnDeathStarted(killer);

	if (UAnimMontage* montage = SelectDeathMontage(killer))
	{
		PlayAnimMontage(montage);
	}
}

UAnimMontage* ACharacterBase::SelectDeathMontage(AActor* killer) const
{
	return DeathMontage;
}