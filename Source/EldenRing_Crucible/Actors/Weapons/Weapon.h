#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Type.h"
#include "Weapon.generated.h"

UCLASS()
class ELDENRING_CRUCIBLE_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	AWeapon();

	void Initialize(ACharacter* character, const class UWeaponDataAsset* data, EWeaponSlot slot);

protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UStaticMeshComponent> Mesh;

public:
	FORCEINLINE class UStaticMeshComponent* GetMesh() const { return Mesh; }
};
