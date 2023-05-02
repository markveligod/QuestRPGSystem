/** Copyright Mark Veligod. Published in 2023. **/

#pragma once

#include "CoreMinimal.h"
#include "RPG_QuestSystemSettingsDataTypes.generated.h"

USTRUCT()
struct FRPG_TaskNodeEditorColor
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(EditAnywhere)
    FLinearColor NodeTitleColor{FLinearColor::Blue};

    UPROPERTY(EditAnywhere)
    FLinearColor NodeCommentColor{FLinearColor::White};

    UPROPERTY(EditAnywhere)
    FLinearColor NodeBodyTintColor{FLinearColor::Blue};
};

USTRUCT()
struct FRPG_SchemePinColor
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere)
    FLinearColor PinTypeColor{FLinearColor::White};

    UPROPERTY(EditAnywhere)
    FLinearColor SecondaryPinTypeColor{FLinearColor::White};
};
