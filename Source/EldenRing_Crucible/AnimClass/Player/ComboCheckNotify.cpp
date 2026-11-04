#include "AnimClass/Player/ComboCheckNotify.h"
#include "Components/Combat/CombatComponents.h"
#include "Components/SkeletalMeshComponent.h"

void UComboCheckNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
    const FAnimNotifyEventReference& EventReference)
{
    Super::Notify(MeshComp, Animation, EventReference);

    AActor* Owner = MeshComp ? MeshComp->GetOwner() : nullptr;
    UCombatComponents* Combat = Owner ? Owner->FindComponentByClass<UCombatComponents>() : nullptr;

    if (Combat)
    {
        Combat->CheckCombo();
    }
}