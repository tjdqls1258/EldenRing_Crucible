#pragma once

#include "CoreMinimal.h"
#include "Actors/Character/CharacterBase.h"
#include "EnemyBase.generated.h"

UCLASS()
class ELDENRING_CRUCIBLE_API AEnemyBase : public ACharacterBase
{
	GENERATED_BODY()

public:
	AEnemyBase();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
