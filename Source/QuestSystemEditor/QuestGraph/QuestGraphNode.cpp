/*=============================================================================
	SoundCueGraphNode.cpp
=============================================================================*/

#include "QuestGraphNode.h"
#include "ScopedTransaction.h"
#include "QuestGraph.h"
#include "Engine/Font.h"
#include "QuestRPGSystem/Objects/QuestObject.h"

#define LOCTEXT_NAMESPACE "SoundCueGraphNode"

/////////////////////////////////////////////////////
// USoundCueGraphNode

UQuestGraphNode::UQuestGraphNode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UQuestGraphNode::PostLoad()
{
	Super::PostLoad();

	// // Fixup any SoundNode back pointers that may be out of date
	// if (QuestNode)
	// {
	// 	QuestNode->GraphNode = this;
	// }
	//
	// for (int32 Index = 0; Index < Pins.Num(); ++Index)
	// {
	// 	UEdGraphPin* Pin = Pins[Index];
	// 	if (Pin->PinName.IsEmpty())
	// 	{
	// 		// Makes sure pin has a name for lookup purposes but user will never see it
	// 		if (Pin->Direction == EGPD_Input)
	// 		{
	// 			Pin->PinName = CreateUniquePinName(TEXT("Input"));
	// 		}
	// 		else
	// 		{
	// 			Pin->PinName = CreateUniquePinName(TEXT("Output"));
	// 		}
	// 		Pin->PinFriendlyName = FText::FromString(TEXT(" "));
	// 	}
	// }
}

void UQuestGraphNode::SetQuestNode(UQuestNode* InQuestNode)
{
	QuestNode = InQuestNode;
	// InQuestNode->GraphNode = this;
}

void UQuestGraphNode::CreateInputPin()
{
	// UEdGraphPin* NewPin = CreatePin(EGPD_Input, TEXT("SoundNode"), TEXT(""), NULL, /*bIsArray=*/ false, /*bIsReference=*/ false, QuestNode->GetInputPinName(GetInputCount()).ToString());
	// if (NewPin->PinName.IsEmpty())
	// {
	// 	// Makes sure pin has a name for lookup purposes but user will never see it
	// 	NewPin->PinName = CreateUniquePinName(TEXT("Input"));
	// 	NewPin->PinFriendlyName = FText::FromString(TEXT(" "));
	// }
}

void UQuestGraphNode::AddInputPin()
{
	const FScopedTransaction Transaction( NSLOCTEXT("UnrealEd", "SoundCueEditorAddInput", "Add Sound Cue Input") );
	Modify();
	CreateInputPin();

	UQuestObject* QuestObject = CastChecked<UQuestGraph>(GetGraph())->GetQuestObject();
	// QuestObject->CompileSoundNodesFromGraphNodes();
	QuestObject->MarkPackageDirty();

	// Refresh the current graph, so the pins can be updated
	GetGraph()->NotifyGraphChanged();
}

void UQuestGraphNode::RemoveInputPin(UEdGraphPin* InGraphPin)
{
	const FScopedTransaction Transaction( NSLOCTEXT("UnrealEd", "SoundCueEditorDeleteInput", "Delete Sound Cue Input") );
	Modify();

	TArray<class UEdGraphPin*> InputPins;
	GetInputPins(InputPins);

	for (int32 InputIndex = 0; InputIndex < InputPins.Num(); InputIndex++)
	{
		if (InGraphPin == InputPins[InputIndex])
		{
			InGraphPin->BreakAllPinLinks();
			Pins.Remove(InGraphPin);
			// also remove the SoundNode child node so ordering matches
			// QuestNode->Modify();
			// QuestNode->RemoveChildNode(InputIndex);
			break;
		}
	}

	UQuestObject* QuestObject = CastChecked<UQuestGraph>(GetGraph())->GetQuestObject();
	// QuestObject->CompileSoundNodesFromGraphNodes();
	QuestObject->MarkPackageDirty();

	// Refresh the current graph, so the pins can be updated
	GetGraph()->NotifyGraphChanged();
}

int32 UQuestGraphNode::EstimateNodeWidth() const
{
	const int32 EstimatedCharWidth = 6;
	FString NodeTitle = GetNodeTitle(ENodeTitleType::FullTitle).ToString();
	UFont* Font = GetDefault<UEditorEngine>()->EditorFont;
	int32 Result = NodeTitle.Len()*EstimatedCharWidth;

	if (Font)
	{
		Result = Font->GetStringSize(*NodeTitle);
	}

	return Result;
}

bool UQuestGraphNode::CanAddInputPin() const
{
	if(QuestNode)
	{
		// Check if adding another input would exceed max child nodes.
		// return QuestNode->ChildNodes.Num() < QuestNode->GetMaxChildNodes();
	}
	return false;
}

FText UQuestGraphNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	if (QuestNode)
	{
		// return QuestNode->GetTitle();
	}
    return Super::GetNodeTitle(TitleType);
}

void UQuestGraphNode::PrepareForCopying()
{
	if (QuestNode)
	{
		// Temporarily take ownership of the SoundNode, so that it is not deleted when cutting
		// QuestNode->Rename(NULL, this, REN_DontCreateRedirectors);
	}
}

void UQuestGraphNode::GetNodeContextMenuActions(UToolMenu* Menu, UGraphNodeContextMenuContext* Context) const
{
    Super::GetNodeContextMenuActions(Menu, Context);
}

void UQuestGraphNode::PostCopyNode()
{
    
}

void UQuestGraphNode::PostEditImport()
{
    
}

void UQuestGraphNode::PostDuplicate(bool bDuplicateForPIE)
{
	Super::PostDuplicate(bDuplicateForPIE);

	if (!bDuplicateForPIE)
	{
		CreateNewGuid();
	}
}

void UQuestGraphNode::CreateInputPins()
{
	// for (int32 ChildIndex = 0; ChildIndex < QuestNode->ChildNodes.Num(); ++ChildIndex)
	// {
	// 	CreateInputPin();
	// }
}

// void UQuestGraphNode::GetContextMenuActions(const FGraphNodeContextMenuBuilder& Context) const
// {
// 	if (Context.Pin)
// 	{
// 		// If on an input that can be deleted, show option
// 		if(Context.Pin->Direction == EGPD_Input && SoundNode->ChildNodes.Num() > SoundNode->GetMinChildNodes())
// 		{
// 			Context.MenuBuilder->AddMenuEntry(FSoundCueGraphEditorCommands::Get().DeleteInput);
// 		}
// 	}
// 	else if (Context.Node)
// 	{
// 		Context.MenuBuilder->BeginSection("SoundCueGraphNodeEdit");
// 		{
// 			Context.MenuBuilder->AddMenuEntry( FGenericCommands::Get().Delete );
// 			Context.MenuBuilder->AddMenuEntry( FGenericCommands::Get().Cut );
// 			Context.MenuBuilder->AddMenuEntry( FGenericCommands::Get().Copy );
// 			Context.MenuBuilder->AddMenuEntry( FGenericCommands::Get().Duplicate );
// 		}
// 		Context.MenuBuilder->EndSection();
//
// 		Context.MenuBuilder->BeginSection("SoundCueGraphNodeAddPlaySync");
// 		{
// 			if (CanAddInputPin())
// 			{
// 				Context.MenuBuilder->AddMenuEntry(FSoundCueGraphEditorCommands::Get().AddInput);
// 			}
//
// 			Context.MenuBuilder->AddMenuEntry(FSoundCueGraphEditorCommands::Get().PlayNode);
//
// 			if ( Cast<USoundNodeWavePlayer>(SoundNode) )
// 			{
// 				Context.MenuBuilder->AddMenuEntry(FSoundCueGraphEditorCommands::Get().BrowserSync);
// 			}
// 		}
// 		Context.MenuBuilder->EndSection();
// 	}
// }

FText UQuestGraphNode::GetTooltipText() const
{
	FText Tooltip;

	if (Tooltip.IsEmpty())
	{
		Tooltip = GetNodeTitle(ENodeTitleType::ListView);
	}
	return Tooltip;
}

FString UQuestGraphNode::GetDocumentationExcerptName() const
{
	// Default the node to searching for an excerpt named for the C++ node class name, including the U prefix.
	// This is done so that the excerpt name in the doc file can be found by find-in-files when searching for the full class name.
	// UClass* MyClass = (QuestNode != NULL) ? QuestNode->GetClass() : this->GetClass();
	// return FString::Printf(TEXT("%s%s"), MyClass->GetPrefixCPP(), *MyClass->GetName());
    return FString();
}

#undef LOCTEXT_NAMESPACE