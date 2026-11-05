#include "Components/Combat/LockOnComponent.h"
#include "Engine/OverlapResult.h"
#include "Engine/World.h"

ULockOnComponent::ULockOnComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void ULockOnComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (controller == nullptr) return;

	if(!IsValid(target) || FVector::Dist(owner->GetActorLocation(), target->GetActorLocation()) > breakDistance)
	{
		SetTarget(FindClosest(FindCandidates()));
		return;
	}

	const FVector targetLocation = target->GetActorLocation() + FVector(0.f, 0.f, targetZoffset);
	const FRotator desired = (targetLocation - owner->GetActorLocation()).Rotation();

	controller->SetControlRotation(FMath::RInterpTo(controller->GetControlRotation(), desired, DeltaTime, cameraSpeed));
}

void ULockOnComponent::BeginPlay()
{
	owner = Cast<APawn>(GetOwner());	
	if (owner)
	{
		controller = owner->GetController();
	}
}

void ULockOnComponent::ToggleLockOn()
{
	SetTarget(target ? nullptr : FindClosest(FindCandidates()));
}

void ULockOnComponent::SwitchTarget(float Direction)
{
	if (!target || FMath::IsNearlyZero(Direction))
	{
		return;
	}
	if (owner == nullptr) return;

	const FVector Origin = owner->GetActorLocation();
	const float CurrentYaw = (target->GetActorLocation() - Origin).Rotation().Yaw;
	const float Sign = FMath::Sign(Direction);

	AActor* Next = nullptr;
	AActor* Wrap = nullptr;
	float BestNext = TNumericLimits<float>::Max();
	float BestWrap = TNumericLimits<float>::Max();

	for (AActor* Candidate : FindCandidates())
	{
		if (Candidate == target)
		{
			continue;
		}

		const float Yaw = (Candidate->GetActorLocation() - Origin).Rotation().Yaw;
		const float Delta = FMath::FindDeltaAngleDegrees(CurrentYaw, Yaw) * Sign;

		if (Delta > 0.f && Delta < BestNext)
		{
			BestNext = Delta;
			Next = Candidate;
		}
		if (Delta < BestWrap)
		{
			BestWrap = Delta; 
			Wrap = Candidate;
		}
	}

	if (AActor* NewTarget = Next ? Next : Wrap)
	{
		SetTarget(NewTarget);
	}
}

TArray<AActor*> ULockOnComponent::FindCandidates() const
{
	TArray<AActor*> result;

	if (owner == nullptr) return result;
	if (!GetWorld()) return result;

	FCollisionQueryParams params(SCENE_QUERY_STAT(LockOnSearch), false, owner);
	TArray<FOverlapResult> overlaps;

	GetWorld()->OverlapMultiByObjectType(
		overlaps,
		owner->GetActorLocation(),
		FQuat::Identity,
		FCollisionObjectQueryParams(ECC_Pawn),
		FCollisionShape::MakeSphere(searchRaius),
		params);

	for (const FOverlapResult& overlap : overlaps)
	{
		AActor* actor = overlap.GetActor();
		if (!actor || !actor->ActorHasTag(TEXT("Enemy"))) continue;

		FHitResult hit;
		FCollisionQueryParams traceParams(SCENE_QUERY_STAT(LockOnSight), false, owner);
		traceParams.AddIgnoredActor(actor);

		const bool bBlocked = GetWorld()->LineTraceSingleByChannel(
			hit,
			owner->GetActorLocation(),
			actor->GetActorLocation(),
			ECC_Visibility,
			traceParams);

		if (!bBlocked)
		{
			result.Add(actor);
		}
	}

	return result;
}

AActor* ULockOnComponent::FindClosest(const TArray<AActor*>& Candidates) const
{
	if (owner == nullptr) return nullptr;

	AActor* closestActor = nullptr;
	float bestDistance = breakDistance * 2.f;

	for(AActor* actor : Candidates)
	{
		if(!actor) continue;
		float distance = FVector::Dist(actor->GetActorLocation(), owner->GetActorLocation());

		if(distance < bestDistance)
		{
			bestDistance = distance;
			closestActor = actor;
		}
	}

	return closestActor;
}

void ULockOnComponent::SetTarget(AActor* NewTarget)
{
	target = NewTarget;
	SetComponentTickEnabled(target != nullptr);
	OnLockOnChanged.Broadcast();
}
