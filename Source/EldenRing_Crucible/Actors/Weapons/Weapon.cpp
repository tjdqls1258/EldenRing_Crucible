#include "Actors/Weapons/Weapon.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"
#include "Assets/Weapon/WeaponDataAsset.h"

AWeapon::AWeapon()
{
    PrimaryActorTick.bCanEverTick = false;

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    SetRootComponent(Mesh);
    Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AWeapon::Initialize(ACharacter* character, const UWeaponDataAsset* data, EWeaponSlot slot)
{
    Mesh->SetStaticMesh(data->Mesh);

    const FName socket = slot == EWeaponSlot::Right
        ? data->RightAttachSocket
        : data->LeftAttachSocket;

    AttachToComponent(character->GetMesh(),
        FAttachmentTransformRules::SnapToTargetNotIncludingScale, socket);
}