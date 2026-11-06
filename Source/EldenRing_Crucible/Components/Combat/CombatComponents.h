#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Type.h"
#include "CombatComponents.generated.h"

class ACharacter;
class AWeapon;
class UAnimMontage;
class UWeaponDataAsset;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ELDENRING_CRUCIBLE_API UCombatComponents : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCombatComponents();

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
	bool bIsAttacking = false;
	bool bComboQueued = false;
	bool bCanQueueCombo = false;
	bool bHitWindowOpen = false;
	
	int32 currentComboStep = 0;
	int32 activePlayId = 0;

	EWeaponSlot AttackSlot = EWeaponSlot::Right;

	TObjectPtr<ACharacter> Character;
	TObjectPtr<UPrimitiveComponent> TraceMesh;

	FHitTraceData ActiveTrace;

	TArray<TObjectPtr<AActor>> HitActors;
	TArray<FVector> PreviousPoints;

	FWeaponSlotData RightSlot;
	FWeaponSlotData LeftSlot;

public:
	void TryAttack(EWeaponSlot slot);
	bool CancelAttack();
	void CheckCombo();

	void OpenComboWindow();
	void CloseComboWindow();

	void BeginHit(FName TraceId);
	void EndHit();

	void EquipWeapon(EWeaponSlot slot, UWeaponDataAsset* data);

protected:
	void PlayCurrentStepAim();
	void HandleMontageEnded(UAnimMontage* Montage, bool bInterrupted, int32 playId);
	void ResetCombo();
	void FaceAttackDirection();

private:
	void PerformHitTrace();

	const TArray<FAttackStep>& GetActiveCombo() const;
	const FHitTraceData* FindHitTrace(FName TraceId) const;

	float GetActiveBaseDamage() const;
	bool CanDamage(const AActor* Victim) const;

	FWeaponSlotData* FindSlot(EWeaponSlot slot);
	const FWeaponSlotData& GetSlot(EWeaponSlot slot) const;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "CombatAnimations")
	TArray<FAttackStep> DefaultCombo; // ¸Ç¼Õ

	UPROPERTY(EditDefaultsOnly, Category = "Combat|Hit")
	TMap<FName, FHitTraceData> BodyTraces;

	UPROPERTY(EditDefaultsOnly, Category = "Combat|Hit")
	FName UnarmedTraceId = NAME_None;

	UPROPERTY(EditDefaultsOnly, Category = "Combat|Hit")
	float DefaultBaseDamage = 10.f;

	UPROPERTY(EditAnywhere, Category = "Combat|Debug")
	bool bDrawHitTrace = false;
	
public:
	FORCEINLINE bool IsAttacking() const { return bIsAttacking; }
};
