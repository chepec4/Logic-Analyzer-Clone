#ifndef SA_LEGACY_INCLUDED
#define SA_LEGACY_INCLUDED

#include "sa.settings.h"

// ............................................................................

namespace sa {
namespace legacy {

using namespace sa::config; // Acceso a Defaults, ParameterCount, etc.

typedef int Preset[ParameterCount];

// Conversor de presets antiguos (v1 -> v2)
bool convertPreset(Preset& value)
{
    using namespace parameters;
    const int n = ParameterCount;

    #define _ ~0
    #define i SettingsIndex +
    
    // Mapa de migración de parámetros
    const Preset maps[] = 
    {
        {   // v1 Mapping
            _, _, _, _, w, h,
            i inputChannel, i peakEnable, i peakDecay,
            i avrgEnable, i avrgTime, i avrgBarType, i avrgBarSize,
            i holdEnable, i holdInfinite, i holdTime, i holdDecay, i holdBarType, i holdBarSize,
            i levelCeil, i levelRange, i levelGrid,
            i freqGridType, i bandsPerOctave,
            i peakBarColor, i holdBarColor, i avrgBarColor,
            i gridBorderColor, i gridLineColor, i gridLabelColor,
            i bkgTopColor, i bkgBottomColor,
            // Nuevos parámetros v2 inicializados a default
            _, // avrgSlope
        }
    };
    #undef i
    #undef _

    int ver = value[version];
    
    // Si ya es la versión actual, no hacer nada
    if (ver == presetVersion) return true;

    // Validación de rango de versión
    if (ver < 1 || ver > (int)(sizeof(maps)/sizeof(*maps)))
        return false;

    const Preset& map = maps[ver - 1];
    Preset src;
    
    // Backup de valores originales
    kali::copy(src, value, n);
    
    // Resetear a defaults modernos
    // [C4 FIX] 'data()' existe gracias a la actualización en sa.settings.h
    kali::copy(value, Defaults().data(), n);
    
    // Migrar valores mapeados
    for (int k = 0; k < n; k++) {
        if (map[k] >= 0) // Si tiene mapeo válido
            value[map[k]] = src[k];
    }

    #if DBG
        trace.warn("%s: Migrated version %i to %i\n", FUNCTION_, ver, presetVersion);
    #endif

    return true;
}

} // ~ namespace legacy
} // ~ namespace sa

#endif // ~ SA_LEGACY_INCLUDED
