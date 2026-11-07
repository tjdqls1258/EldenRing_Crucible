#include "Components/Combat/EquipmentComponent.h"
#include "GameFramework/Character.h"
#include "Engine/World.h"
#include "Actors/Weapons/Weapon.h"
#include "Assets/Weapon/WeaponDataAsset.h"

UEquipmentComponent::UEquipmentComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UEquipmentComponent::BeginPlay()
{
	Super::BeginPlay();

	Character = Cast<ACharacter>(GetOwner());

	// 목록의 첫 무기 장착
	if (!RightWeapons.IsEmpty())
	{
		EquipWeapon(EWeaponSlot::Right, RightWeapons[0]);
	}

	if (!LeftWeapons.IsEmpty())
	{
		EquipWeapon(EWeaponSlot::Left, LeftWeapons[0]);
	}
}

void UEquipmentComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	DestroySlotActor(RightSlot);
	DestroySlotActor(LeftSlot);

	Super::EndPlay(EndPlayReason);
}

void UEquipmentComponent::EquipWeapon(EWeaponSlot slot, UWeaponDataAsset* data)
{
	if (!Character)
	{
		return;
	}

	FWeaponSlotData* Target = FindSlot(slot);
	DestroySlotActor(*Target);
	Target->Data = data;

	if (!data)
	{
		return; // 맨손
	}

	FActorSpawnParameters Params;
	Params.Owner = Character;
	Params.Instigator = Character;

	Target->Actor = GetWorld()->SpawnActor<AWeapon>(AWeapon::StaticClass(), FTransform::Identity, Params);
	if (!Target->Actor)
	{
		Target->Data = nullptr;
		return;
	}

	Target->Actor->Initialize(Character, data, slot);
}

void UEquipmentComponent::CycleWeapon(EWeaponSlot slot)
{
	TArray<UWeaponDataAsset*> Cycle;
	BuildCycleList(slot, Cycle);

	if (Cycle.Num() <= 1)
	{
		return; // 무기 없음, 맨손 고정
	}

	int32& Index = slot == EWeaponSlot::Right ? RightWeaponIndex : LeftWeaponIndex;
	Index = (Index + 1) % Cycle.Num();
	EquipWeapon(slot, Cycle[Index]);
}

const UWeaponDataAsset* UEquipmentComponent::GetWeaponData(EWeaponSlot slot) const
{
	return GetSlot(slot).Data;
}

UPrimitiveComponent* UEquipmentComponent::GetWeaponMesh(EWeaponSlot slot) const
{
	const FWeaponSlotData& Slot = GetSlot(slot);
	return Slot.Actor ? Slot.Actor->GetMesh() : nullptr;
}

FWeaponSlotData* UEquipmentComponent::FindSlot(EWeaponSlot slot)
{
	return slot == EWeaponSlot::Right ? &RightSlot : &LeftSlot;
}

const FWeaponSlotData& UEquipmentComponent::GetSlot(EWeaponSlot slot) const
{
	return slot == EWeaponSlot::Right ? RightSlot : LeftSlot;
}

void UEquipmentComponent::DestroySlotActor(FWeaponSlotData& slotData)
{
	if (slotData.Actor)
	{
		slotData.Actor->Destroy();
	}

	slotData.Actor = nullptr;
	slotData.Data = nullptr;
}

void UEquipmentComponent::BuildCycleList(EWeaponSlot slot, TArray<UWeaponDataAsset*>& outList) const
{
	const TArray<TObjectPtr<UWeaponDataAsset>>& List =
		slot == EWeaponSlot::Right ? RightWeapons : LeftWeapons;

	outList.Reset();
	for (UWeaponDataAsset* Weapon : List)
	{
		if (Weapon)
		{
			outList.Add(Weapon);
		}
	}

	outList.Add(nullptr); // 마지막은 항상 맨손
}
