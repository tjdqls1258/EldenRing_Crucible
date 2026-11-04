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
    // 추후 전투에 데미지 배율 수치 추가
};
