/** Copyright Mark Veligod. Published in 2023. **/

#include "RPG_QuestSystemStyle.h"
#include "ClassIconFinder.h"
#include "Styling/SlateStyleRegistry.h"
#include "Misc/Paths.h"

static const FVector2D Icon16x16(16.0f, 16.0f);
static const FVector2D Icon20x20(20.0f, 20.0f);
static const FVector2D Icon24x24(24.0f, 24.0f);
static const FVector2D Icon40x40(40.0f, 40.0f);
static const FVector2D Icon32x32(32.0f, 32.0f);
static const FVector2D Icon64x64(64.0f, 64.0f);
static const FVector2D Icon96x96(96.0f, 96.0f);

TSharedPtr<FSlateStyleSet> FRPG_QuestSystemStyle::StyleSet = nullptr;

void FRPG_QuestSystemStyle::SetClassIcon(const FName ClassName, const WIDECHAR* RelativePath, const FLinearColor ClassColor)
{
    StyleSet->Set(FName("ClassIcon." + ClassName.ToString()),
        new FSlateImageBrush(StyleSet->RootToContentDir(FString(RelativePath) + "_16x", TEXT(".png")), Icon16x16, ClassColor)
    );
    StyleSet->Set(FName("ClassThumbnail." + ClassName.ToString()),
        new FSlateImageBrush(StyleSet->RootToContentDir(FString(RelativePath) + "_64x", TEXT(".png")), Icon64x64, ClassColor)
    );
}

void FRPG_QuestSystemStyle::Initialize()
{
    if (StyleSet.IsValid()) return; 
    
    StyleSet = MakeShareable(new FSlateStyleSet("FRPG_QuestSystemStyle"));
    StyleSet->SetContentRoot(FPaths::ProjectPluginsDir() / TEXT("RPG_QuestSystem/Resources"));

    SetClassIcon("RPG_QuestObjectBase", TEXT("AssetIcons/QuestSystemIcon"), FLinearColor::White);
    SetClassIcon("RPG_QuestManagerBase", TEXT("AssetIcons/QuestSystemIcon"), FLinearColor::White);
    FSlateStyleRegistry::RegisterSlateStyle(*StyleSet.Get());
}

void FRPG_QuestSystemStyle::Shutdown()
{
    if (!StyleSet.IsValid()) return;

    FSlateStyleRegistry::UnRegisterSlateStyle(*StyleSet.Get());
    ensure(StyleSet.IsUnique());
    StyleSet.Reset();
}
