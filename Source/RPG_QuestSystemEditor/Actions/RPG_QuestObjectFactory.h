// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "RPG_QuestObjectFactory.generated.h"

/**
 * 
 */
UCLASS()
class RPG_QUESTSYSTEMEDITOR_API URPG_QuestObjectFactory : public UFactory
{
    GENERATED_BODY()

    URPG_QuestObjectFactory();

    // UFactory interface
    virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
    virtual uint32 GetMenuCategories() const override;
    virtual FText GetDisplayName() const override;
    virtual FString GetDefaultNewAssetName() const override;
    // End of UFactory interface

};
