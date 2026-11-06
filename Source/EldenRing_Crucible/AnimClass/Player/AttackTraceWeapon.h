#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AttackTraceWeapon.generated.h"

UCLASS()
class ELDENRING_CRUCIBLE_API UAttackTraceWeapon : public UAnimNotifyState
{
	GENERATED_BODY()
private:
	UPROPERTY(EditAnywhere, Category = "Hit")
	FName TraceId = NAME_None;

protected:
	virtual void NotifyBegin(USkeletalMeshComponent* mesh, UAnimSequenceBase* animation, float totalDuration, const FAnimNotifyEventReference& eventReference) override;

	virtual void NotifyEnd(USkeletalMeshComponent* mesh, UAnimSequenceBase* animation, const FAnimNotifyEventReference& eventReference) override;
};