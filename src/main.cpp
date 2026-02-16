#include "includes.h"
#include "main.h"

#define VST_EXPORT extern "C" __declspec(dllexport)

// Punto de entrada moderno
VST_EXPORT AEffect* VSTPluginMain(audioMasterCallback audioMaster) {
    if (!audioMaster) return nullptr;
    try {
        Plugin* plugin = new Plugin(audioMaster);
        return plugin->getAeffect();
    } catch (...) { return nullptr; }
}

// [C4 MASTER FIX] Punto de entrada legacy requerido por algunos hosts
// Debe retornar int (intptr_t cast) para evitar el error de GCC
VST_EXPORT int main(audioMasterCallback audioMaster) {
    AEffect* e = VSTPluginMain(audioMaster);
    return (int)(intptr_t)e;
}
