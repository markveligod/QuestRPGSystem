#pragma once
#include "EditorStyleSet.h"


/*-----------------------------------------------------------------------------
   FQuestGraphEditorCommands
-----------------------------------------------------------------------------*/

class FQuestGraphEditorCommands : public TCommands<FQuestGraphEditorCommands>
{
public:
    /** Constructor */
    FQuestGraphEditorCommands() 
        : TCommands<FQuestGraphEditorCommands>("QuestGraphEditor", NSLOCTEXT("Contexts", "QuestGraphEditor", "Quest Graph Editor"), NAME_None, FAppStyle::GetAppStyleSetName())
    {
    }
	
    /** Plays the SoundCue */
    TSharedPtr<FUICommandInfo> PlayCue;
	
    /** Stops the currently playing cue/node */
    TSharedPtr<FUICommandInfo> StopCueNode;

    /** Plays the selected node */
    TSharedPtr<FUICommandInfo> PlayNode;

    /** Plays the SoundCue or stops the currently playing cue/node */
    TSharedPtr<FUICommandInfo> TogglePlayback;

    /** Selects the SoundWave in the content browser */
    TSharedPtr<FUICommandInfo> BrowserSync;

    /** Breaks the node input/output link */
    TSharedPtr<FUICommandInfo> BreakLink;

    /** Adds an input to the node */
    TSharedPtr<FUICommandInfo> AddInput;

    /** Removes an input from the node */
    TSharedPtr<FUICommandInfo> DeleteInput;

    /** Initialize commands */
    virtual void RegisterCommands() override;
};