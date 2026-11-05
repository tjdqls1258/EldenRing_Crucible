#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LockOnComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLockOnChanged);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ELDENRING_CRUCIBLE_API ULockOnComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	ULockOnComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void BeginPlay() override;	

public:
	void ToggleLockOn();
	void SwitchTarget(float Direction);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "LockOn")
	float searchRaius = 1500.0f;
	UPROPERTY(EditDefaultsOnly, Category = "LockOn")
	float breakDistance = 1800.0f;
	UPROPERTY(EditDefaultsOnly, Category = "LockOn")
	float cameraSpeed = 8.0f;
	UPROPERTY(EditDefaultsOnly, Category = "LockOn", meta = (ToolTip = "적을 바라볼 Z 위치로 - 으로 해야 시야가 편함."))
	float targetZoffset = -50.0f;

private:
	TObjectPtr<class AActor> target;

	class APawn* owner;
	class AController* controller;
private:
	TArray<class AActor*> FindCandidates() const;
	class AActor* FindClosest(const TArray<class AActor*>& Candidates) const;
	void SetTarget(AActor* NewTarget);

public:
	FOnLockOnChanged OnLockOnChanged; //타겟 교체 이벤트

public:
	FORCEINLINE bool IsLockedOn() const { return target != nullptr; }
};
