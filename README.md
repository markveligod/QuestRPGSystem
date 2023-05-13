# Plugin RPG_QuestSystem
## Project Description
This plugin is implemented with the basic implementation of the quest system with its own quest editor.  
The purpose of the plugin is to provide the simplest possible way to implement this type of quests in games.  
A lot of control functions are virtual to override the logic of behavior.

Supported versions of the Unreal Engine: UE5.1, UE5.2  
The supported versions correspond to the branch names. For example: UE-5.1-Release  

## Quest
The quest object consists of 3 sections:  
- Details
- Quest Graph
- List Tasks

### Location creation of a quest object
![image](https://github.com/markveligod/QuestRPGSystem/assets/34418187/f0778ffd-48f1-4ebe-a771-fc576b512e9b)

### The insides of the quest object
![image](https://github.com/markveligod/QuestRPGSystem/assets/34418187/a4868ad4-e3e1-4866-a27c-7678b7968654)

### List quest table
![image](https://github.com/markveligod/QuestRPGSystem/assets/34418187/c133ef29-3b64-41ba-88cf-505ae5e259b3)

## Example tasks
- BroadcastTag - Notification of receipt of the gameplay tag
- Delay - Timer Delay Task
- Finish Node - Node of completion of the current quest object
- InstanceTask - Run task with sub tasks
- MoveTo - The task of moving to the point
- PrintDebugString - The task of displaying a line on the screen

## 📫 Other <a name="Other"></a>
:bangbang: Attention: If you can improve my trash code then make a pull request.  

**:copyright:Authors:**  
  
*[Mark Veligod](https://github.com/markveligod)*  
