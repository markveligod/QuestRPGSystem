/** Copyright Mark Veligod. Published in 2023. **/

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "RPG_QuestSystemDataTypes.generated.h"

/** @enum The general state of the entity **/
UENUM(BlueprintType)
enum class ERPG_StateEntity : uint8
{
    None,
    Init,
    Run,
    Complete
};

/** @enum Custom verb to logging for quest system **/
enum ERPG_QSLogVerb
{
    Display,
    Warning,
    Error
};

/** @struct Data for saving the current state of the quest **/
USTRUCT()
struct FRPG_DataSaveQuest
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(SaveGame)
    FName QuestRowNameTable{NAME_None};

    UPROPERTY(SaveGame)
    FString DataQuestJSON{""};

    void Serialize(FArchive& Ar)
    {
        Ar << QuestRowNameTable << DataQuestJSON;
    }

    friend FArchive& operator<<(FArchive& Ar, FRPG_DataSaveQuest& InstanceData)
    {
        Ar << InstanceData.QuestRowNameTable;
        Ar << InstanceData.DataQuestJSON;
        return Ar;
    }
};

/** @struct Data structure for storing information about the quest and for replicating to the client **/
USTRUCT()
struct FRPG_DataQuest
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY()
    FName QuestRowNameTable{NAME_None};

    UPROPERTY()
    class URPG_QuestObjectBase* ActiveQuest{nullptr};

    bool IsValidQuest() const
    {
        return QuestRowNameTable != NAME_None && ActiveQuest != nullptr;
    }

    bool operator==(const FRPG_DataQuest& Data) const
    {
        return Data.QuestRowNameTable.IsEqual(QuestRowNameTable);
    }
};

/** @struct Data structure for storing the basic information of the quest in the form of a data table **/
USTRUCT(BlueprintType)
struct FRPG_DataQuestTable : public FTableRowBase
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FText NameQuest;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (MultiLine = true))
    FText DescriptionQuest;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSoftObjectPtr<URPG_QuestObjectBase> QuestObjectPath;
};
