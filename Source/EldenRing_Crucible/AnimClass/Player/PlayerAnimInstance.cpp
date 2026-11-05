#include "PlayerAnimInstance.h"
#include "Components/Combat/CombatComponents.h"
#include "GameFramework/Character.h"

void UPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	if (owningCharacter != nullptr)
	{
		combatComponents = Cast<UCombatComponents>(owningCharacter->GetComponentByClass(UCombatComponents::StaticClass()));
	}
}
