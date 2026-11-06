#include "AnimClass/Player/AttackTraceWeapon.h"
#include "Components/Combat/CombatComponents.h"

void UAttackTraceWeapon::NotifyBegin(USkeletalMeshComponent* mesh, UAnimSequenceBase* animation, float totalDuration, const FAnimNotifyEventReference& eventReference)
{
	Super::NotifyBegin(mesh, animation, totalDuration, eventReference);

	AActor* owner = mesh ? mesh->GetOwner() : nullptr;
	UCombatComponents* combatComponent = owner ? owner->FindComponentByClass<UCombatComponents>() : nullptr;
	
	if (!combatComponent) return;

	combatComponent->BeginHit(TraceId);
}

void UAttackTraceWeapon::NotifyEnd(USkeletalMeshComponent* mesh, UAnimSequenceBase* animation, const FAnimNotifyEventReference& eventReference)
{
	Super::NotifyEnd(mesh, animation, eventReference);

	AActor* owner = mesh ? mesh->GetOwner() : nullptr;
	UCombatComponents* combatComponent = owner ? owner->FindComponentByClass<UCombatComponents>() : nullptr;

	if (!combatComponent) return;

	combatComponent->EndHit();
}