#include "AnimClass/Player/ComboCheckNotify.h"
#include "Components/Combat/CombatComponents.h"
#include "Components/SkeletalMeshComponent.h"

void UComboCheckNotify::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
    float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
    Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

    if (UCombatComponents* Combat = FindCombat(MeshComp))
    {
        Combat->OpenComboWindow();
    }
}

void UComboCheckNotify::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
    const FAnimNotifyEventReference& EventReference)
{
    Super::NotifyEnd(MeshComp, Animation, EventReference);

    if (UCombatComponents* Combat = FindCombat(MeshComp))
    {
        Combat->CloseComboWindow();
    }
}

UCombatComponents* UComboCheckNotify::FindCombat(USkeletalMeshComponent* MeshComp) const
{
    AActor* Owner = MeshComp ? MeshComp->GetOwner() : nullptr;
    return Owner ? Owner->FindComponentByClass<UCombatComponents>() : nullptr;
}