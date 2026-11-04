#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Type.h"
#include "CombatComponents.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ELDENRING_CRUCIBLE_API UCombatComponents : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCombatComponents();

protected:
	virtual void BeginPlay() override;

protected:
	bool bIsAttacking;
	bool bComboQueued;

	int32 currentComboStep;
	int32 activePlayId;

	class ACharacter* Character;
	class UAnimInstance* AnimInstance;

public:
	void TryAttack();
	bool CancelAttack();

	void CheckCombo();

protected:
	void PlayCurrentStepAim();
	void HandleMontageEnded(UAnimMontage* Montage, bool bInterrupted, int32 playId);
	void ResetCombo();
	void FaceAttackDirection();
private:
	const TArray<FAttackStep>& GetActiveCombo() const;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "CombatAnimations")
	TArray<FAttackStep> DefaultCombo; // ¸Ç¼Õ
	
public:
	FORCEINLINE bool IsAttacking() const { return bIsAttacking; }
};
