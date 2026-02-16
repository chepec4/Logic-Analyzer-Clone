#ifndef SA_LEGACY_INCLUDED
#define SA_LEGACY_INCLUDED

#include "sa.settings.h"

namespace kali   {
namespace legacy {

using namespace sa::config;

typedef int Preset[ParameterCount];

bool convertPreset(Preset& value)
{
    using namespace parameters;
    const int n = ParameterCount;

    #define _ ~0
    #define i SettingsIndex +
    const Preset maps[] = 
    {
        {
            _, _, _, _, w, h,
            i inputChannel, i peakEnable, i peakDecay,
            i avrgEnable, i avrgTime, i avrgBarType, i avrgBarSize,
            i holdEnable, i holdInfinite, i holdTime, i holdDecay, i holdBarType, i holdBarSize,
            i levelCeil, i levelRange, i levelGrid,
            i freqGridType, i bandsPerOctave,
            i peakBarColor, i holdBarColor, i avrgBarColor,
            i gridBorderColor, i gridLineColor, i gridLabelColor,
            i bkgTopColor, i bkgBottomColor,
            _ // Padding para llegar a 32 elementos (avrgSlope no estaba en v1)
        }
    };
    #undef i
    #undef _

    int ver = value[version];
    if (ver == presetVersion) return true;

    if (ver < 1 || ver > (int)(sizeof(maps)/sizeof(*maps))) return false;

    const Preset& map = maps[ver - 1];
    Preset src;
    std::memcpy(src, value, sizeof(Preset));
    std::memcpy(value, Defaults().data(), sizeof(Preset));
    for (int j = 0; j < n; j++)
        if (map[j] >= 0)
            value[map[j]] = src[j];

    return true;    
}

} // ~ legacy
} // ~ kali

#endif // SA_LEGACY_INCLUDED
