#pragma once

#include "CoreMinimal.h"
#include "AnimClass/CharacterAnimInstanceBase.h"
#include "PlayerAnimInstance.generated.h"

UCLASS()
class ELDENRING_CRUCIBLE_API UPlayerAnimInstance : public UCharacterAnimInstanceBase
{
	GENERATED_BODY()
	// 착용한 장비, 한손, 양손 등에 따라 추가 필요.
protected:
	virtual void NativeInitializeAnimation() override;

private:
	class UCombatComponents* combatComponents;
};
