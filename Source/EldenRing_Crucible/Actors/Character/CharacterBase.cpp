#include "Actors/Character/CharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/Stat/HealthComponent.h"
#include "Components/Combat/CombatComponents.h"
#include "Components/Combat/EquipmentComponent.h"
#include "Components/CapsuleComponent.h"

ACharacterBase::ACharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;

	combatComponent = CreateDefaultSubobject<UCombatComponents>(TEXT("CombatComponent"));
	equipmentComponents = CreateDefaultSubobject<UEquipmentComponent>(TEXT("EquipmentCompoents"));
	healthComponents = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponents"));
}

void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();

	UCombatComponents* Found =
		FindComponentByClass<UCombatComponents>();

	healthComponents->OnDeath.AddDynamic(this, &ThisClass::HandleDeath);
}

void ACharacterBase::HandleDeath(AActor* killer)
{
	if (combatComponent)
	{
		combatComponent->CancelAttack();
	}

	GetCharacterMovement()->DisableMovement();
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	OnDeathStarted(killer);

	if (UAnimMontage* montage = SelectDeathMontage(killer))
	{
		PlayAnimMontage(montage);
	}
}

void ACharacterBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (!IsValid(combatComponent))
	{
		combatComponent =
			FindComponentByClass<UCombatComponents>();
	}

	ensureMsgf(
		IsValid(combatComponent),
		TEXT("CombatComponent is nullptr!")
	);
}

UAnimMontage* ACharacterBase::SelectDeathMontage(AActor* killer) const
{
	return DeathMontage;
}