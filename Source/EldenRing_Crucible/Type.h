// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Type.generated.h"

USTRUCT(BlueprintType)
struct FAttackStep
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly)
    TObjectPtr<class UAnimMontage> Montage;

	UPROPERTY(EditDefaultsOnly)
    bool bCanCancel;


    UPROPERTY(EditDefaultsOnly)
	float DamageMultiplier = 1.0f;
};

USTRUCT(BlueprintType)
struct FHitTraceData
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly) FName StartSocket;
    UPROPERTY(EditDefaultsOnly) FName EndSocket;

public:
    UPROPERTY(EditDefaultsOnly, meta = (ClampMin = "0.0", ToolTip = "0이면 선, 아니면 원형")) float Radius = 0.f;
    UPROPERTY(EditDefaultsOnly, meta = (ClampMin = "1")) int32 SampleCount = 1;
};

USTRUCT()
struct FWeaponSlotData
{
    GENERATED_BODY()

    UPROPERTY() TObjectPtr<class UWeaponDataAsset> Data;
    UPROPERTY() TObjectPtr<class AWeapon> Actor;
};

UENUM(BlueprintType)
enum class EWeaponGripRule : uint8 
{
    Default, 
    TwoHandOnly //추후 양손 잡기 추가
};

UENUM(BlueprintType)
enum class EWeaponGrip : uint8 
{
    OneHand,
    TwoHand //추후 양손 잡기 추가
};

UENUM(BlueprintType)
enum class EWeaponSlot : uint8 
{
    Right,
    Left 
};