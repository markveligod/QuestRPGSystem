/*=============================================================================
	QuestCueGraphNode_Root.cpp
=============================================================================*/

#include "QuestGraphNode_Root.h"
#include "ScopedTransaction.h"

#define LOCTEXT_NAMESPACE "QuestGraphNode_Root"

/////////////////////////////////////////////////////
// UQuestGraphNode_Root

UQuestGraphNode_Root::UQuestGraphNode_Root(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

#pragma region ObjectInterface

void UQuestGraphNode_Root::PostLoad()
{
	Super::PostLoad();
    
}

void UQuestGraphNode_Root::PrepareForCopying()
{
	
}

void UQuestGraphNode_Root::GetNodeContextMenuActions(UToolMenu* Menu, UGraphNodeContextMenuContext* Context) const
{
    Super::GetNodeContextMenuActions(Menu, Context);
}

void UQuestGraphNode_Root::PostEditImport()
{
    
}

void UQuestGraphNode_Root::PostDuplicate(bool bDuplicateForPIE)
{
	Super::PostDuplicate(bDuplicateForPIE);

	if (!bDuplicateForPIE)
	{
		CreateNewGuid();
	}
}

#pragma endregion

#pragma region EdGraphNodeInterface

void UQuestGraphNode_Root::AllocateDefaultPins()
{
    Super::AllocateDefaultPins();

    CreatePin(EGPD_Output, TEXT("QuestNode"), TEXT(""), nullptr, /*bIsArray=*/ false, /*bIsReference=*/ false, TEXT("Start Node Quest"));
}

void UQuestGraphNode_Root::ReconstructNode()
{
    Super::ReconstructNode();
}

void UQuestGraphNode_Root::AutowireNewNode(UEdGraphPin* FromPin)
{
    Super::AutowireNewNode(FromPin);
}

bool UQuestGraphNode_Root::CanCreateUnderSpecifiedSchema(const UEdGraphSchema* Schema) const
{
    return Super::CanCreateUnderSpecifiedSchema(Schema);
}

FString UQuestGraphNode_Root::GetDocumentationLink() const
{
    return Super::GetDocumentationLink();
}


FText UQuestGraphNode_Root::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
    return Super::GetNodeTitle(TitleType);
}

FText UQuestGraphNode_Root::GetTooltipText() const
{
	FText Tooltip;

	if (Tooltip.IsEmpty())
	{
		Tooltip = GetNodeTitle(ENodeTitleType::ListView);
	}
	return Tooltip;
}

FString UQuestGraphNode_Root::GetDocumentationExcerptName() const
{
    return FString();
}

#pragma endregion

#undef LOCTEXT_NAMESPACE
