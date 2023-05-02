/** Copyright Mark Veligod. Published in 2023. **/

#include "RPG_QuestSystem.h"

#define LOCTEXT_NAMESPACE "FRPG_QuestSystemModule"

void FRPG_QuestSystemModule::StartupModule()
{
    // This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FRPG_QuestSystemModule::ShutdownModule()
{
    // This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
    // we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FRPG_QuestSystemModule, RPG_QuestSystem)