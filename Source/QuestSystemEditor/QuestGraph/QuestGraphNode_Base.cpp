/*=============================================================================
	QuestGraphNode_Base.cpp
=============================================================================*/

#include "QuestGraphNode_Base.h"
#include "QuestEdGraphSchema.h"

/////////////////////////////////////////////////////
// UQuestGraphNode_Base

UQuestGraphNode_Base::UQuestGraphNode_Base(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

#pragma region ObjectInterface

void UQuestGraphNode_Base::PostLoad()
{
    Super::PostLoad();
}

void UQuestGraphNode_Base::PostEditImport()
{
    Super::PostEditImport();
}

void UQuestGraphNode_Base::PostDuplicate(bool bDuplicateForPIE)
{
    Super::PostDuplicate(bDuplicateForPIE);
}

#pragma endregion

#pragma region EdGraphNodeInterface

void UQuestGraphNode_Base::AllocateDefaultPins()
{
    Super::AllocateDefaultPins();
    CreatePin(EGPD_Input, TEXT("QuestNode"), TEXT(""), nullptr, /*bIsArray=*/ false, /*bIsReference=*/ false, TEXT("Input Node Quest"));
}

void UQuestGraphNode_Base::ReconstructNode()
{
	// Break any links to 'orphan' pins
	for (int32 PinIndex = 0; PinIndex < Pins.Num(); ++PinIndex)
	{
		UEdGraphPin* Pin = Pins[PinIndex];
		TArray<class UEdGraphPin*>& LinkedToRef = Pin->LinkedTo;
		for (int32 LinkIdx=0; LinkIdx < LinkedToRef.Num(); LinkIdx++)
		{
			UEdGraphPin* OtherPin = LinkedToRef[LinkIdx];
			// If we are linked to a pin that its owner doesn't know about, break that link
			if (!OtherPin->GetOwningNode()->Pins.Contains(OtherPin))
			{
				Pin->LinkedTo.Remove(OtherPin);
			}
		}
	}

	// // Store the old Input and Output pins
	// TArray<UEdGraphPin*> OldInputPins;
	// GetInputPins(OldInputPins);
	// UEdGraphPin* OldOutputPin = GetOutputPin();
	//
	// // Move the existing pins to a saved array
	// TArray<UEdGraphPin*> OldPins(Pins);
	// Pins.Empty();
	//
	// // Recreate the new pins
	// AllocateDefaultPins();
	//
	// // Get new Input and Output pins
	// TArray<UEdGraphPin*> NewInputPins;
	// GetInputPins(NewInputPins);
	// UEdGraphPin* NewOutputPin = GetOutputPin();
	//
	// for (int32 PinIndex = 0; PinIndex < OldInputPins.Num(); PinIndex++)
	// {
	// 	if (PinIndex < NewInputPins.Num())
	// 	{
	// 		NewInputPins[PinIndex]->CopyPersistentDataFromOldPin(*OldInputPins[PinIndex]);
	// 	}
	// }
	//
	// NewOutputPin->CopyPersistentDataFromOldPin(*OldOutputPin);
	// OldInputPins.Empty();
	// OldOutputPin = nullptr;
	//
	// // Throw away the original pins
	// for (int32 OldPinIndex = 0; OldPinIndex < OldPins.Num(); ++OldPinIndex)
	// {
	// 	UEdGraphPin* OldPin = OldPins[OldPinIndex];
	// 	OldPin->Modify();
	// 	OldPin->BreakAllPinLinks();

// #if 0
// 		UEdGraphNode::ReturnPinToPool(OldPin);
// #else
// 		OldPin->Rename(NULL, GetTransientPackage(), REN_None);
// 		OldPin->RemoveFromRoot();
// 		OldPin->MarkPendingKill();
// #endif
	// }
	// OldPins.Empty();
}

void UQuestGraphNode_Base::AutowireNewNode(UEdGraphPin* FromPin)
{
	if (FromPin != nullptr)
	{
		const UQuestGraphSchema* Schema = CastChecked<UQuestGraphSchema>(GetSchema());

		TSet<UEdGraphNode*> NodeList;

		// auto-connect from dragged pin to first compatible pin on the new node
		for (int32 i=0; i<Pins.Num(); i++)
		{
			UEdGraphPin* Pin = Pins[i];
			check(Pin);
			FPinConnectionResponse Response = Schema->CanCreateConnection(FromPin, Pin);
			if (ECanCreateConnectionResponse::CONNECT_RESPONSE_MAKE == Response.Response)
			{
				if (Schema->TryCreateConnection(FromPin, Pin))
				{
					NodeList.Add(FromPin->GetOwningNode());
					NodeList.Add(this);
				}
				break;
			}
			else if(ECanCreateConnectionResponse::CONNECT_RESPONSE_BREAK_OTHERS_A == Response.Response)
			{
				break;
			}
		}

		// Send all nodes that received a new pin connection a notification
		for (auto It = NodeList.CreateConstIterator(); It; ++It)
		{
			UEdGraphNode* Node = (*It);
			Node->NodeConnectionListChanged();
		}
	}
}

bool UQuestGraphNode_Base::CanCreateUnderSpecifiedSchema(const UEdGraphSchema* Schema) const
{
	return Schema->IsA(UQuestGraphSchema::StaticClass());
}

FString UQuestGraphNode_Base::GetDocumentationLink() const
{
	return TEXT("");
}

FText UQuestGraphNode_Base::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
    return Super::GetNodeTitle(TitleType);
}

void UQuestGraphNode_Base::PrepareForCopying()
{
    Super::PrepareForCopying();
}

void UQuestGraphNode_Base::GetNodeContextMenuActions(UToolMenu* Menu, UGraphNodeContextMenuContext* Context) const
{
    Super::GetNodeContextMenuActions(Menu, Context);
}

FText UQuestGraphNode_Base::GetTooltipText() const
{
    return Super::GetTooltipText();
}

FString UQuestGraphNode_Base::GetDocumentationExcerptName() const
{
    return Super::GetDocumentationExcerptName();
}

#pragma endregion
