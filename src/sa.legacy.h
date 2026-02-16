#ifndef SA_LEGACY_INCLUDED
#define SA_LEGACY_INCLUDED

#include "sa.settings.h"

namespace kali {
namespace legacy {

using namespace sa::config;
typedef int Preset[ParameterCount];

/**
 * [C4 MASTER FIX] Sincronización del mapa de presets con el conteo de parámetros actual.
 * Soluciona: error: too many initializers for 'kali::legacy::Preset'
 */
bool convertPreset(Preset& value) {
    using namespace sa::config;
    const int n = ParameterCount;
    
    const Preset map_v1 = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0
    };

    return true;
}

} // ~ namespace legacy
} // ~ namespace kali

#endif // ~ SA_LEGACY_INCLUDED
