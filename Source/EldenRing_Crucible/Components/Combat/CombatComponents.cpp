#include "Components/Combat/CombatComponents.h"
#include "GameFramework/Character.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/DamageType.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "Assets/Weapon/WeaponDataAsset.h"
#include "Components/Combat/EquipmentComponent.h"

UCombatComponents::UCombatComponents()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}


void UCombatComponents::BeginPlay()
{
	Super::BeginPlay();

	Character = CastChecked<ACharacter>(GetOwner());
	Equipment = GetOwner()->FindComponentByClass<UEquipmentComponent>();
}

void UCombatComponents::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bHitWindowOpen)
	{
		PerformHitTrace();
	}
}

void UCombatComponents::TryAttack(EWeaponSlot slot)
{
	if (bIsAttacking)
	{
		if (bCanQueueCombo && slot == AttackSlot)
		{
			bComboQueued = true;
		}
		return;
	}

	AttackSlot = slot;

	if (GetActiveCombo().IsEmpty())
	{
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("Try Attack"));
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
	if (!Combo.IsValidIndex(currentComboStep) || !Combo[currentComboStep].bCanCancel)
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
	UAnimInstance* AnimInstance = Character ? Character->GetMesh()->GetAnimInstance() : nullptr;

	if (!AnimInstance || !Combo.IsValidIndex(currentComboStep) || !Combo[currentComboStep].Montage)
	{
		ResetCombo();
		return;
	}

	UAnimMontage* Montage = Combo[currentComboStep].Montage;
	EndHit();
	bComboQueued = false;
	bCanQueueCombo = false;
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

void UCombatComponents::OpenComboWindow()
{
	bCanQueueCombo = true;
}

void UCombatComponents::CloseComboWindow()
{
	bCanQueueCombo = false;
	CheckCombo();
}

void UCombatComponents::BeginHit(FName TraceId)
{
	if (!Character || !bIsAttacking)
	{
		return;
	}

	const FHitTraceData* Trace = FindHitTrace(TraceId);
	if (!Trace)
	{
		UE_LOG(LogTemp, Warning, TEXT("Can't Find HitTrace: %s"), *TraceId.ToString());
		return;
	}

	UPrimitiveComponent* WeaponMesh = Equipment ? Equipment->GetWeaponMesh(AttackSlot) : nullptr;
	TraceMesh = (TraceId.IsNone() && WeaponMesh)
		? WeaponMesh
		: static_cast<UPrimitiveComponent*>(Character->GetMesh());

	if (!TraceMesh->DoesSocketExist(Trace->StartSocket) || !TraceMesh->DoesSocketExist(Trace->EndSocket))
	{
		UE_LOG(LogTemp, Warning, TEXT("No Socket: %s, %s (%s)"),
			*Trace->StartSocket.ToString(), *Trace->EndSocket.ToString(), *GetNameSafe(TraceMesh));
		TraceMesh = nullptr;
		return;
	}

	ActiveTrace = *Trace;
	HitActors.Reset();
	PreviousPoints.SetNum(FMath::Max(ActiveTrace.SampleCount, 1));

	const FVector Start = TraceMesh->GetSocketLocation(ActiveTrace.StartSocket);
	const FVector End = TraceMesh->GetSocketLocation(ActiveTrace.EndSocket);
	for (int32 i = 0; i < PreviousPoints.Num(); ++i)
	{
		const float Alpha = PreviousPoints.Num() > 1
			? static_cast<float>(i) / (PreviousPoints.Num() - 1)
			: 0.f;
		PreviousPoints[i] = FMath::Lerp(Start, End, Alpha);
	}

	bHitWindowOpen = true;
	SetComponentTickEnabled(true);
}

void UCombatComponents::EndHit()
{
	bHitWindowOpen = false;
	HitActors.Reset();
	PreviousPoints.Reset();
	TraceMesh = nullptr;
	SetComponentTickEnabled(false);
}

void UCombatComponents::PerformHitTrace()
{
	if (!Character || !TraceMesh)
	{
		return;
	}

	const FVector Start = TraceMesh->GetSocketLocation(ActiveTrace.StartSocket);
	const FVector End = TraceMesh->GetSocketLocation(ActiveTrace.EndSocket);

	const TArray<FAttackStep>& Combo = GetActiveCombo();
	const float Multiplier = Combo.IsValidIndex(currentComboStep)
		? Combo[currentComboStep].DamageMultiplier
		: 1.f;
	const float Damage = GetActiveBaseDamage() * Multiplier;

	FCollisionQueryParams Params(SCENE_QUERY_STAT(AttackTrace), false, Character);

	for (int32 i = 0; i < PreviousPoints.Num(); ++i)
	{
		const float Alpha = PreviousPoints.Num() > 1
			? static_cast<float>(i) / (PreviousPoints.Num() - 1)
			: 0.f;
		const FVector Current = FMath::Lerp(Start, End, Alpha);

		TArray<FHitResult> Hits;
		if (ActiveTrace.Radius > 0.f)
		{
			GetWorld()->SweepMultiByChannel(
				Hits, PreviousPoints[i], Current, FQuat::Identity,
				ECC_GameTraceChannel1,
				FCollisionShape::MakeSphere(ActiveTrace.Radius), Params);
		}
		else
		{
			GetWorld()->LineTraceMultiByChannel(
				Hits, PreviousPoints[i], Current, ECC_GameTraceChannel1, Params);
		}

		//충돌 디버그
#if ENABLE_DRAW_DEBUG 
		if (bDrawHitTrace)
		{
			DrawDebugLine(GetWorld(), PreviousPoints[i], Current,
				Hits.IsEmpty() ? NoHitColor : HitColor, false, 1.f);
		}
#endif

		for (const FHitResult& Hit : Hits)
		{
			AActor* Victim = Hit.GetActor();
			if (!CanDamage(Victim))
			{
				continue;
			}

			HitActors.Add(Victim);
			UGameplayStatics::ApplyDamage(
				Victim, Damage, Character->GetController(),
				Character, UDamageType::StaticClass());
		}

		PreviousPoints[i] = Current;
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
	EndHit();
	bIsAttacking = false;
	bComboQueued = false;
	bCanQueueCombo = false;
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
	const UWeaponDataAsset* Data = GetAttackWeaponData();
	if (!Data)
	{
		return DefaultCombo;
	}

	const TArray<FAttackStep>& Combo = AttackSlot == EWeaponSlot::Right
		? Data->RightHandCombo
		: Data->LeftHandCombo;

	return Combo.IsEmpty() ? DefaultCombo : Combo;
}

const FHitTraceData* UCombatComponents::FindHitTrace(FName TraceId) const
{
	if (TraceId.IsNone())
	{
		const UWeaponDataAsset* Data = GetAttackWeaponData();
		return Data ? &Data->HitTrace : BodyTraces.Find(UnarmedTraceId);
	}
	return BodyTraces.Find(TraceId);
}

const UWeaponDataAsset* UCombatComponents::GetAttackWeaponData() const
{
	return Equipment ? Equipment->GetWeaponData(AttackSlot) : nullptr;
}

float UCombatComponents::GetActiveBaseDamage() const
{
	const UWeaponDataAsset* Data = GetAttackWeaponData();
	return Data ? Data->BaseDamage : DefaultBaseDamage;
}

bool UCombatComponents::CanDamage(const AActor* Victim) const
{
	if (!Victim || Victim == GetOwner() || HitActors.Contains(Victim))
	{
		return false;
	}

	UE_LOG(LogTemp, Warning, TEXT("Can Damage: %d %d"), GetOwner()->ActorHasTag(CanDamageTag), Victim->ActorHasTag(CanDamageTag));
	return GetOwner()->ActorHasTag(CanDamageTag) != Victim->ActorHasTag(CanDamageTag);
}