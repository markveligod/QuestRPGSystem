/** Copyright Mark Veligod. Published in 2023. **/

#include "RPG_ExampleObject/QuestTriggerAdd.h"
#include "Components/TextRenderComponent.h"
#include "GameFramework/Character.h"
#include "RPG_Components/RPG_QuestManagerBase.h"

AQuestTriggerAddBox::AQuestTriggerAddBox()
{
    TextRender = CreateDefaultSubobject<UTextRenderComponent>(FName("Render Text"));
    if (TextRender)
    {
        TextRender->SetupAttachment(GetRootComponent());
        TextRender->SetHorizontalAlignment(EHTA_Center);
        TextRender->SetVerticalAlignment(EVRTA_TextCenter);
        TextRender->SetWorldSize(WorldSizeText);
        TextRender->bHiddenInGame = false;
    }
    SetHidden(false);
    bNetLoadOnClient = true;
}

void AQuestTriggerAddBox::NotifyActorBeginOverlap(AActor* OtherActor)
{
    Super::NotifyActorBeginOverlap(OtherActor);

    if (!HasAuthority()) return;
    if (!OtherActor) return;
    if (!OtherActor->IsA(ACharacter::StaticClass())) return;
    APlayerController* PC = OtherActor->GetInstigatorController<APlayerController>();
    if (!PC) return;
    URPG_QuestManagerBase* QM = PC->FindComponentByClass<URPG_QuestManagerBase>();
    if (!QM) return;
    if (QM->GetStateQuestByName(Quest.RowName) == ERPG_StateEntity::None)
    {
        QM->ServerAddQuest(Quest.RowName);
    }
}

#if WITH_EDITOR

void AQuestTriggerAddBox::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    if (PropertyChangedEvent.MemberProperty && TextRender)
    {
        const FName PropertyName = PropertyChangedEvent.MemberProperty->GetFName();

        if (PropertyName == GET_MEMBER_NAME_CHECKED(AQuestTriggerAddBox, Quest))
        {
            TextRender->SetText(FText::FromName(Quest.RowName));
        }
        if (PropertyName == GET_MEMBER_NAME_CHECKED(AQuestTriggerAddBox, WorldSizeText))
        {
            TextRender->SetWorldSize(WorldSizeText);
        }
        if (PropertyName == GET_MEMBER_NAME_CHECKED(AQuestTriggerAddBox, TextColor))
        {
            TextRender->SetTextRenderColor(TextColor);
        }
    }
}

#endif
