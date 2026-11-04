#include "Components/Combat/CombatComponents.h"
#include "GameFramework/Character.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

UCombatComponents::UCombatComponents()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}


void UCombatComponents::BeginPlay()
{
	Super::BeginPlay();

	Character = CastChecked<ACharacter>(GetOwner());


	if (Character != nullptr)
	{
		AnimInstance = Character->GetMesh()->GetAnimInstance();
	}
}

void UCombatComponents::TryAttack()
{
	//UE_LOG(LogTemp, Warning, TEXT("TryAttack 호출"));
	if (bIsAttacking)
	{
		bComboQueued = true; // 다음 타 예약
		UE_LOG(LogTemp, Warning, TEXT("rest next"));
		return;
	}

	if (GetActiveCombo().IsEmpty())
	{
		return;
	}

	currentComboStep = 0;
	PlayCurrentStepAim();
}

bool UCombatComponents::CancelAttack()
{
	if (!bIsAttacking || !Character)
	{
		return false;
	}

	const TArray<FAttackStep>& Combo = GetActiveCombo();
	if ((!AnimInstance || !Combo.IsValidIndex(currentComboStep)) || !Combo[currentComboStep].bCanCancel)
	{
		return false;
	}

	++activePlayId;
	ResetCombo();

	if (UAnimInstance* Anim = Character->GetMesh()->GetAnimInstance())
	{
		Anim->Montage_Stop(0.1f);
	}

	return true;
}

void UCombatComponents::PlayCurrentStepAim()
{
	const TArray<FAttackStep>& Combo = GetActiveCombo();

	if (!AnimInstance || !Combo.IsValidIndex(currentComboStep) || !Combo[currentComboStep].Montage)
	{
		ResetCombo();
		return;
	}

	UAnimMontage* Montage = Combo[currentComboStep].Montage;
	bComboQueued = false;
	FaceAttackDirection();
	Character->GetCharacterMovement()->StopMovementImmediately();

	// 이전 몽타주의 종료 이벤트와 구분
	const int32 PlayId = ++activePlayId;
	if (AnimInstance->Montage_Play(Montage) <= 0.f)
	{
		ResetCombo();
		return;
	}

	bIsAttacking = true;

	FOnMontageEnded EndDelegate;
	EndDelegate.BindUObject(this, &UCombatComponents::HandleMontageEnded, PlayId);
	AnimInstance->Montage_SetEndDelegate(EndDelegate, Montage);
}

void UCombatComponents::CheckCombo()
{
	UE_LOG(LogTemp, Warning, TEXT("CheckCombo - Attacking:%d, Queued:%d, Step:%d, Count:%d"),
		bIsAttacking, bComboQueued, currentComboStep, GetActiveCombo().Num());

	if (!bIsAttacking || !bComboQueued)
	{
		return;
	}

	if (GetActiveCombo().IsValidIndex(currentComboStep + 1))
	{
		++currentComboStep;
		PlayCurrentStepAim();
	}
	else
	{
		bComboQueued = false;
	}
}

void UCombatComponents::HandleMontageEnded(UAnimMontage* Montage, bool bInterrupted, int32 playId)
{
	if (playId == activePlayId)
	{
		ResetCombo();
	}
}

void UCombatComponents::ResetCombo()
{
	bIsAttacking = false;
	bComboQueued = false;
	currentComboStep = 0;
}

void UCombatComponents::FaceAttackDirection()
{
	if (!Character || !Character->IsPlayerControlled())
	{
		return; // 몬스터는 AI가 방향을 결정
	}

	FRotator NewRotation = Character->GetActorRotation();
	NewRotation.Yaw = Character->GetControlRotation().Yaw;
	Character->SetActorRotation(NewRotation);
}

const TArray<FAttackStep>& UCombatComponents::GetActiveCombo() const
{
	// 무기 추가 예정
	return DefaultCombo;
}
