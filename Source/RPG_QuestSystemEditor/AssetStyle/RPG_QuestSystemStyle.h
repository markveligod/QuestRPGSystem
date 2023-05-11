/** Copyright Mark Veligod. Published in 2023. **/

#pragma once

#include "CoreMinimal.h"
#include "Styling/SlateStyle.h"

class FRPG_QuestSystemStyle
{
public:

    static void Initialize();
    static void Shutdown();


    static void SetClassIcon(const FName ClassName, const WIDECHAR* RelativePath, const FLinearColor ClassColor);
    static FORCEINLINE const FName& GetStyleSetName() { return StyleSet->GetStyleSetName(); }
    static FORCEINLINE TSharedPtr<ISlateStyle> Get() { return StyleSet; }

    template<typename WidgetStyleType>
    static FORCEINLINE const WidgetStyleType& GetWidgetStyle(const FName& PropertyName, const ANSICHAR* Specifier = nullptr)
    {
        return StyleSet->GetWidgetStyle<WidgetStyleType>(PropertyName, Specifier);
    }


private:

    static TSharedPtr<FSlateStyleSet> StyleSet;
};
