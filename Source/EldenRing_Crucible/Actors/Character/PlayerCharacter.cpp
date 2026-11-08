#include "PlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/Combat/CombatComponents.h"
#include "Components/Combat/LockOnComponent.h"
#include "Components/Combat/EquipmentComponent.h"
#include "Components/Stat/HealthComponent.h"

APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

    springArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    springArm->SetupAttachment(RootComponent);
    springArm->TargetArmLength = 400.0f;
    springArm->bUsePawnControlRotation = true;

    camera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    camera->SetupAttachment(springArm, USpringArmComponent::SocketName);
    camera->bUsePawnControlRotation = false;

    lockOnComponents = CreateDefaultSubobject<ULockOnComponent>(TEXT("LockOnComponents"));

    bUseControllerRotationYaw = false;
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

    if (APlayerController* PC = Cast<APlayerController>(Controller))
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
            ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
        {

            if (MappingContext)
            {
                Subsystem->AddMappingContext(MappingContext, 0);
            }
        }
    }

    if(GetCharacterMovement() != nullptr)
    {
        GetCharacterMovement()->MaxWalkSpeed = speed;
	}

	lockOnComponents->OnLockOnChanged.AddDynamic(this, &ThisClass::UpdateRotationMode);
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (UEnhancedInputComponent* eic = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
		eic->BindAction(MoveAction,         ETriggerEvent::Triggered,this, &ThisClass::Move             );
		eic->BindAction(LookAction,         ETriggerEvent::Triggered,this, &ThisClass::Look             );
        eic->BindAction(JumpAction,         ETriggerEvent::Started,  this, &ThisClass::JumpFunc         );
        eic->BindAction(RightAttackAction,  ETriggerEvent::Started,  this, &ThisClass::RightAttack      );
        eic->BindAction(LeftAttackAction,   ETriggerEvent::Started,  this, &ThisClass::LeftAttack       );
        eic->BindAction(LockOnAction,       ETriggerEvent::Started,  this, &ThisClass::LockOn           );
        eic->BindAction(LockOnSwitchAction, ETriggerEvent::Started,  this, &ThisClass::LockOnSwitch     );
        eic->BindAction(SprintAction,       ETriggerEvent::Triggered,this, &ThisClass::SprintStart      ); 
        eic->BindAction(SprintAction,       ETriggerEvent::Completed,this, &ThisClass::SprintEnd        );
        eic->BindAction(DodgeAction,        ETriggerEvent::Triggered,this, &ThisClass::DodgeTrigger     );
        eic->BindAction(SwapWeaponAction,   ETriggerEvent::Started,  this, &ThisClass::SwapWeapon       );
    }
}

void APlayerCharacter::Landed(const FHitResult& Hit)
{
    Super::Landed(Hit);
    if (GetCharacterMovement() == nullptr) return;
    if (GetWorld() == nullptr) return;

    bIsLanding = true;
    GetCharacterMovement()->StopMovementImmediately();
    GetWorld()->GetTimerManager().SetTimer(landTimer, this,
        &ThisClass::EndLanding, landLockTime, false);
}

float APlayerCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    return 0.0f;
}

void APlayerCharacter::Move(const FInputActionValue& value)
{
    if (!Controller || bIsLanding) return;
    if (!combatComponents) return;
    if (combatComponents->IsAttacking()) return;

    const FVector2D dir = value.Get<FVector2D>();

    const FRotator rot = Controller->GetControlRotation();
    const FRotator yrot(0, rot.Yaw, 0);

    const FVector forward = FRotationMatrix(yrot).GetUnitAxis(EAxis::X);
    const FVector right = FRotationMatrix(yrot).GetUnitAxis(EAxis::Y);

    AddMovementInput(forward, dir.X);
    AddMovementInput(right, dir.Y);
}

void APlayerCharacter::Look(const FInputActionValue& value)
{
    if (lockOnComponents->IsLockedOn()) return;
    const FVector2D look = value.Get<FVector2D>();

    AddControllerYawInput(look.X);
    AddControllerPitchInput(look.Y);
}

void APlayerCharacter::RightAttack(const FInputActionValue& value)
{
    combatComponents->TryAttack(EWeaponSlot::Right);
}

void APlayerCharacter::LeftAttack(const FInputActionValue& value)
{
    combatComponents->TryAttack(EWeaponSlot::Left);
}

void APlayerCharacter::JumpFunc(const FInputActionValue& value)
{
    if (!combatComponents) return;
    if (combatComponents->IsAttacking()) return;
    Jump();
}

void APlayerCharacter::LockOn(const FInputActionValue& value)
{
    lockOnComponents->ToggleLockOn();
}

void APlayerCharacter::SprintStart(const FInputActionValue& value)
{
    if (bIsSprinting) return;

    bIsSprinting = true;
    GetCharacterMovement()->MaxWalkSpeed = speed * speedMultiplier;
    UpdateRotationMode();
}

void APlayerCharacter::DodgeTrigger(const FInputActionValue& value)
{
    if (!combatComponents) return;
    if (combatComponents->IsAttacking() && !combatComponents->CancelAttack()) return;
    //±¸¸£±â
}

void APlayerCharacter::SprintEnd(const FInputActionValue& value)
{
    bIsSprinting = false;
    GetCharacterMovement()->MaxWalkSpeed = speed;
    UpdateRotationMode();
}

void APlayerCharacter::LockOnSwitch(const FInputActionValue& value)
{
    if (lockOnComponents == nullptr) return;

	lockOnComponents->SwitchTarget(value.Get<float>());
}

void APlayerCharacter::SwapWeapon(const FInputActionValue& value)
{
    if (combatComponents->IsAttacking()) return;

    equipmentComponents->CycleWeapon(value.Get<float>() > 0.f ? EWeaponSlot::Right : EWeaponSlot::Left);
}

void APlayerCharacter::EndLanding()
{
    bIsLanding = false;
}

void APlayerCharacter::UpdateRotationMode()
{
    if (lockOnComponents == nullptr) return;

    const bool bStrafe = lockOnComponents->IsLockedOn() && !bIsSprinting;
    GetCharacterMovement()->bOrientRotationToMovement = !bStrafe;
    GetCharacterMovement()->bUseControllerDesiredRotation = bStrafe;
}
