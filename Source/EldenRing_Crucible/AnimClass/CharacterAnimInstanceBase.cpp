#include "AnimClass/CharacterAnimInstanceBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"

void UCharacterAnimInstanceBase::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	owningCharacter = Cast<ACharacter>(TryGetPawnOwner());

	if (owningCharacter)
	{
		movement = owningCharacter->GetCharacterMovement();
	}
}

void UCharacterAnimInstanceBase::NativeUpdateAnimation(float deltaSeconds)
{
	Super::NativeUpdateAnimation(deltaSeconds);

	if (!owningCharacter || !movement) return;

	{//Speed
		const FVector velocity = owningCharacter->GetVelocity();
		speed = velocity.Size2D();
		velocityZ = velocity.Z;

		{//Direction
			direction = CalculateDirection(velocity, owningCharacter->GetActorRotation());
		}
	}
	
	{//bIsFalling
		bIsFalling = movement->IsFalling();
	}
}
