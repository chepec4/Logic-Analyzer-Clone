#include "includes.h"
#include "main.h"
#include "kali/app.dll.h"

/**
 * [C4 MASTER FIX] Punto de entrada nativo para VST 2.4.
 * Debe retornar int (el puntero casteado de AEffect) para máxima compatibilidad.
 */

#define VST_EXPORT extern "C" __declspec(dllexport)

VST_EXPORT AEffect* VSTPluginMain(audioMasterCallback audioMaster) {
    if (!audioMaster) return nullptr;
    try {
        Plugin* plugin = new Plugin(audioMaster);
        return plugin->getAeffect();
    } catch (...) { return nullptr; }
}

// Alias para hosts antiguos
VST_EXPORT int main(audioMasterCallback audioMaster) {
    AEffect* e = VSTPluginMain(audioMaster);
    return (int)(intptr_t)e;
}
