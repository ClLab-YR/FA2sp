#include "Body.h"

#include <Helpers/Macro.h>

#include "../../Helpers/ControlHelpers.h"
#include "../../Helpers/Translations.h"

DEFINE_HOOK(502E66, CTriggerOption_OnInitDialog_RepeatTypeFix, 7)
{
    GET(CTriggerOption*, pThis, ESI);

    pThis->CCBHouse.DeleteAllStrings();
    pThis->CCBRepeatType.DeleteAllStrings();

    pThis->CCBRepeatType.AddString(Translations::TranslateOrDefault("TriggerRepeatType.OneTimeOr", "0 - One Time OR"));
    pThis->CCBRepeatType.AddString(Translations::TranslateOrDefault("TriggerRepeatType.OneTimeAnd", "1 - One Time AND"));
    pThis->CCBRepeatType.AddString(Translations::TranslateOrDefault("TriggerRepeatType.RepeatingOr", "2 - Repeating OR"));

    return 0;
}

DEFINE_HOOK(501EAD, CTriggerOption_OnCBHouseSelChanged, 5)
{
    return 0x501EB2;
}