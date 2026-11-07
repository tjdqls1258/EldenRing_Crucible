#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Type.h"
#include "EquipmentComponent.generated.h"

class ACharacter;
class UPrimitiveComponent;
class UWeaponDataAsset;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ELDENRING_CRUCIBLE_API UEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UEquipmentComponent();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
	UPROPERTY(Transient)
	TObjectPtr<ACharacter> Character;

	UPROPERTY(Transient)
	FWeaponSlotData RightSlot;

	UPROPERTY(Transient)
	FWeaponSlotData LeftSlot;

private:
	int32 RightWeaponIndex = 0;
	int32 LeftWeaponIndex = 0;

public:
	void EquipWeapon(EWeaponSlot slot, UWeaponDataAsset* data);
	void CycleWeapon(EWeaponSlot slot);

	const UWeaponDataAsset* GetWeaponData(EWeaponSlot slot) const;
	UPrimitiveComponent* GetWeaponMesh(EWeaponSlot slot) const;

private:
	FWeaponSlotData* FindSlot(EWeaponSlot slot);
	const FWeaponSlotData& GetSlot(EWeaponSlot slot) const;

	void DestroySlotActor(FWeaponSlotData& slotData);
	void BuildCycleList(EWeaponSlot slot, TArray<UWeaponDataAsset*>& outList) const;

protected:
	// 비어 있는 항목은 맨손
	UPROPERTY(EditDefaultsOnly, Category = "Equipment")
	TArray<TObjectPtr<UWeaponDataAsset>> RightWeapons;

	UPROPERTY(EditDefaultsOnly, Category = "Equipment")
	TArray<TObjectPtr<UWeaponDataAsset>> LeftWeapons;

};
