#include "PlayerAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"

void UPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	owningCharacter = Cast<ACharacter>(TryGetPawnOwner());

	if (owningCharacter)
	{
		movement = owningCharacter->GetCharacterMovement();
	}
}

void UPlayerAnimInstance::NativeUpdateAnimation(float deltaSeconds)
{
	Super::NativeUpdateAnimation(deltaSeconds);

	if (!owningCharacter || !movement) return;

	{
		// Speed 설정
		FVector velocity = owningCharacter->GetVelocity();
		velocity.Z = 0;
		speed = velocity.Size();
	}

	{
		direction = CalculateDirection(owningCharacter->GetVelocity(), owningCharacter->GetActorRotation());
	}

	{
		// bIsFalling 설정
		bIsFalling = movement->IsFalling();
		velocityZ = owningCharacter->GetVelocity().Z;
	}
}
