#pragma once

#include "Factories/Factory.h"
#include "QuestFactory.generated.h"

/**
 * 
 */
UCLASS()
class QUESTSYSTEMEDITOR_API UQuestFactory : public UFactory
{
    GENERATED_BODY()

    UQuestFactory();

    // UFactory interface
    virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
    // End of UFactory interface

    virtual FText GetDisplayName() const override;
    virtual FString GetDefaultNewAssetName() const override;
};
