#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Type.h"
#include "WeaponDataAsset.generated.h"

UCLASS()
class ELDENRING_CRUCIBLE_API UWeaponDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly, Category = "Visual")
    TObjectPtr<UStaticMesh> Mesh;

    UPROPERTY(EditDefaultsOnly, Category = "Visual")
    FName RightAttachSocket = NAME_None; 

    UPROPERTY(EditDefaultsOnly, Category = "Visual")
    FName LeftAttachSocket = NAME_None;

    UPROPERTY(EditDefaultsOnly, Category = "Visual")
    FTransform RightAttachOffset;

    UPROPERTY(EditDefaultsOnly, Category = "Visual")
    FTransform LeftAttachOffset;

    UPROPERTY(EditDefaultsOnly, Category = "Combat")
    float BaseDamage = 20.f;

    UPROPERTY(EditDefaultsOnly, Category = "Combat")
    TArray<FAttackStep> RightHandCombo;

    UPROPERTY(EditDefaultsOnly, Category = "Combat")
    TArray<FAttackStep> LeftHandCombo;

    UPROPERTY(EditDefaultsOnly, Category = "Combat")
    FHitTraceData HitTrace; //무기 트레이스 데이터

    UPROPERTY(EditDefaultsOnly, Category = "Combat")
    EWeaponGripRule GripRule = EWeaponGripRule::Default;
};
