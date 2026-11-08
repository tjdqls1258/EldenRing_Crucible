#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CharacterBase.generated.h"

class UAnimMontage;

UCLASS()
class ELDENRING_CRUCIBLE_API ACharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	ACharacterBase();

protected:
	virtual void BeginPlay() override;
	virtual void OnDeathStarted(AActor* killer) {}
	UAnimMontage* SelectDeathMontage(AActor* killer) const;

protected:
	UFUNCTION()
	void HandleDeath(AActor* killer);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UCombatComponents> combatComponents;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<class UEquipmentComponent> equipmentComponents;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat")
	TObjectPtr<class UHealthComponent> healthComponents;

	UPROPERTY(EditDefaultsOnly, Category = "Death")
	TObjectPtr<UAnimMontage> DeathMontage;
};
