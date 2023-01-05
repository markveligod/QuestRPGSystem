#include "QuestFactory.h"
#include "QuestRPGSystem/Objects/QuestObject.h"

FText UQuestFactory::GetDisplayName() const
{
    return FText::FromString("Quest");
}

FString UQuestFactory::GetDefaultNewAssetName() const
{
    return FString(TEXT("NewQuest"));
}

UQuestFactory::UQuestFactory()
{
    SupportedClass = UQuestObject::StaticClass();
    bCreateNew = true;
    bEditAfterNew = true;
    bEditorImport = false;
}

UObject* UQuestFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
    UQuestObject* NewObjectAsset = NewObject<UQuestObject>(InParent, Class, Name, Flags | RF_Transactional);
    return NewObjectAsset;
}
