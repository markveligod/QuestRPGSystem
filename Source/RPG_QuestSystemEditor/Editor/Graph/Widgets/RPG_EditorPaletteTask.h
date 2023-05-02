/** Copyright Mark Veligod. Published in 2023. **/

#pragma once

#include "SGraphPalette.h"

class SActionEditorPaletteTask : public SGraphPaletteItem
{
public:
    SLATE_BEGIN_ARGS(SActionEditorPaletteTask) {}
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs, FCreateWidgetForActionData* const InCreateData);

};

class SEditorPaletteTasks : public SGraphPalette
{

public:

    SLATE_BEGIN_ARGS(SEditorPaletteTasks) {}
    SLATE_ARGUMENT(TWeakPtr<class FRPG_QuestAssetEditor>, InEditor)
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs, TWeakPtr<class FRPG_QuestAssetEditor> InEditor);

    virtual void RefreshActionsList(bool bPreserveExpansion) override;
	
protected:

    virtual TSharedRef<SWidget> OnCreateWidgetForAction(FCreateWidgetForActionData* const InCreateData) override;
    virtual void CollectAllActions(FGraphActionListBuilderBase& OutAllActions) override;

    void OnActionSelected(const TArray<TSharedPtr<FEdGraphSchemaAction>>& InActions, ESelectInfo::Type InSelectionType);

private:
    TWeakPtr<class FRPG_QuestAssetEditor> Editor;
    TArray<UClass*> TaskNodes;

};

