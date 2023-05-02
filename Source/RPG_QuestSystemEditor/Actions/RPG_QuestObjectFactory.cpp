// Fill out your copyright notice in the Description page of Project Settings.

#include "RPG_QuestObjectFactory.h"

#include "RPG_QuestSystem/RPG_Config/RPG_QuestSystemSettings.h"
#include "RPG_QuestSystem/RPG_QuestObjects/RPG_QuestObjectBase.h"
#include "RPG_QuestSystemEditor/RPG_QuestSystemEditor.h"

#define LOCTEXT_NAMESPACE "QuestObjectFactory"

URPG_QuestObjectFactory::URPG_QuestObjectFactory()
{
    bCreateNew = true;
    bEditAfterNew = true;

    const URPG_QuestSystemSettings* QuestSystemSettings = GetDefault<URPG_QuestSystemSettings>();
    SupportedClass = QuestSystemSettings ? QuestSystemSettings->SupportQuestClass : URPG_QuestObjectBase::StaticClass();
}

UObject* URPG_QuestObjectFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
    URPG_QuestObjectBase* NewObjectAsset = NewObject<URPG_QuestObjectBase>(InParent, Class, Name, Flags | RF_Transactional);
    return NewObjectAsset;
}

uint32 URPG_QuestObjectFactory::GetMenuCategories() const
{
    return FRPG_QuestSystemEditorModule::AssetCategory;
}

FText URPG_QuestObjectFactory::GetDisplayName() const
{
    return LOCTEXT("QuestObjectText", "Quest");
}

FString URPG_QuestObjectFactory::GetDefaultNewAssetName() const
{
    return FString(TEXT("NewQuest"));
}

#undef LOCTEXT_NAMESPACE
