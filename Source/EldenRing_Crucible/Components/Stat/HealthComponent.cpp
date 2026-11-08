#include "Components/Stat/HealthComponent.h"

UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentHealth = MaxHealth;

	AActor* owner = GetOwner();
	if (owner)
	{
		owner->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::HandleTakeDamage);
	}
}

void UHealthComponent::Heal(float amount)
{
	if (bIsDead || amount <= 0.f)
	{
		return;
	}


	SetHealth(CurrentHealth + amount, nullptr);
}

void UHealthComponent::SetInvincible(bool bInInvincible)
{
	bInvincible = bInInvincible;
}

void UHealthComponent::HandleTakeDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (bIsDead || bInvincible || Damage <= 0.f)
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("GetDamage : %f, CurrentHp : %f"), Damage, CurrentHealth);

	SetHealth(CurrentHealth - Damage, DamageCauser);

	UE_LOG(LogTemp, Warning, TEXT("-> CurrentHp : %f"), CurrentHealth);
}

void UHealthComponent::SetHealth(float newHealth, AActor* instigator)
{
	CurrentHealth = FMath::Clamp(newHealth, 0.f, MaxHealth);
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth, instigator);

	if (CurrentHealth <= 0.f && !bIsDead)
	{
		bIsDead = true;
		OnDeath.Broadcast(instigator);
	}
}

