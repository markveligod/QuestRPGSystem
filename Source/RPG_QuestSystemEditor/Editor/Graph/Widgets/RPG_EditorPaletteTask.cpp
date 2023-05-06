/** Copyright Mark Veligod. Published in 2023. **/

#include "RPG_EditorPaletteTask.h"
#include "RPG_QuestSystem/RPG_TaskNodes/RPG_TaskNodeBase.h"
#include "RPG_QuestSystemEditor/Editor/FRPG_QuestAssetEditor.h"
#include "RPG_QuestSystemEditor/Editor/Graph/GraphActions/QuestActionsDataTypes.h"

#define LOCTEXT_NAMESPACE "ActionEditorPaletteTask"

void SActionEditorPaletteTask::Construct(const FArguments& InArgs, FCreateWidgetForActionData* const InCreateData)
{
    const FSlateFontInfo NameFont = FCoreStyle::GetDefaultFontStyle("Regular", 10);

    check(InCreateData->Action.IsValid());

    const TSharedPtr<FEdGraphSchemaAction> GraphAction = InCreateData->Action;
    ActionPtr = InCreateData->Action;

    const FSlateBrush* IconBrush = FAppStyle::GetBrush(TEXT("NoBrush"));
    const FSlateColor IconColor = FSlateColor::UseForeground();
    const FText IconToolTip = GraphAction->GetTooltipDescription();

    const TSharedRef<SWidget> IconWidget = CreateIconWidget(IconToolTip, IconBrush, IconColor);
    const TSharedRef<SWidget> NameSlotWidget = CreateTextSlotWidget(InCreateData, false);

    // Create the actual widget
    this->ChildSlot[SNew(SHorizontalBox) + SHorizontalBox::Slot().AutoWidth()[IconWidget] + SHorizontalBox::Slot().FillWidth(1.f).VAlign(VAlign_Center).Padding(3, 0)[NameSlotWidget]];
}

void SEditorPaletteTasks::Construct(const FArguments& InArgs, TWeakPtr<class FRPG_QuestAssetEditor> InEditor)
{
    Editor = InEditor;
    this->ChildSlot[SNew(SBorder).Padding(2.0f).BorderImage(
        FAppStyle::GetBrush("ToolPanel.GroupBorder"))[SNew(SVerticalBox) +
                                                      SVerticalBox::Slot()  // Filter UI
                                                          .AutoHeight()

                                                      + SVerticalBox::Slot()  // Content list
                                                            .HAlign(HAlign_Fill)
                                                            .VAlign(VAlign_Fill)[SAssignNew(GraphActionMenu, SGraphActionMenu)
                                                                                     .OnActionDragged(this, &SEditorPaletteTasks::OnActionDragged)
                                                                                     .OnActionSelected(this, &SEditorPaletteTasks::OnActionSelected)
                                                                                     .OnCreateWidgetForAction(this, &SEditorPaletteTasks::OnCreateWidgetForAction)
                                                                                     .OnCollectAllActions(this, &SEditorPaletteTasks::CollectAllActions)
                                                                                     .AutoExpandActionMenu(true)]]];
}

void SEditorPaletteTasks::RefreshActionsList(bool bPreserveExpansion)
{

    SGraphPalette::RefreshActionsList(bPreserveExpansion);
}

TSharedRef<SWidget> SEditorPaletteTasks::OnCreateWidgetForAction(FCreateWidgetForActionData* const InCreateData)
{
    return SNew(SActionEditorPaletteTask, InCreateData);
}

void SEditorPaletteTasks::CollectAllActions(FGraphActionListBuilderBase& OutAllActions)
{
    if (!Editor.IsValid()) return;
    GetDerivedClasses(URPG_TaskNodeBase::StaticClass(), TaskNodes);

    FGraphActionMenuBuilder ActionMenuBuilder;
    for (const UClass* NodeClass : TaskNodes)
    {
        if (URPG_TaskNodeBase* Task = Cast<URPG_TaskNodeBase>(NodeClass->GetDefaultObject()))
        {
            const FText NameCategoryTask = FText::FromString(Editor.Pin()->GetQuestBeingEdited()->GetCategoryNameByTypeNode(ERPG_TypeNode::StandardNode));
            TSharedPtr<FEdGraphSchemaAction_Task> NewNodeActionTask(new FEdGraphSchemaAction_Task(NameCategoryTask, Task, ERPG_TypeNode::StandardNode));
            ActionMenuBuilder.AddAction(NewNodeActionTask);
        }
    }

    const FText NameCategoryFinishNode = FText::FromString(Editor.Pin()->GetQuestBeingEdited()->GetCategoryNameByTypeNode(ERPG_TypeNode::FinishNode));
    const TSharedPtr<FEdGraphSchemaAction_Task> NewNodeActionFinishNode(new FEdGraphSchemaAction_Task(NameCategoryFinishNode, FText::FromString("Finish Node"), FText::FromString("Finish Node"), ERPG_TypeNode::FinishNode));
    ActionMenuBuilder.AddAction(NewNodeActionFinishNode);
    OutAllActions.Append(ActionMenuBuilder);
}

void SEditorPaletteTasks::OnActionSelected(const TArray<TSharedPtr<FEdGraphSchemaAction>>& InActions, ESelectInfo::Type InSelectionType)
{
    UE_LOG(LogTemp, Display, TEXT("Action selected"));
    if (InActions.IsValidIndex(0))
    {
        FEdGraphSchemaAction_Task* Action = static_cast<FEdGraphSchemaAction_Task*>(InActions[0].Get());
        if (Action && Action->NodeClass)
        {
        }
    }
}

#undef LOCTEXT_NAMESPACE
