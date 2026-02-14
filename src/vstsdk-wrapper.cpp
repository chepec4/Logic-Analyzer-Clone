#include "main.h" // Vital que vaya primero
#include "includes.h"

// Desactivar warnings de Steinberg
#pragma warning(push, 3)
#include "audioeffect.cpp"
#include "audioeffectx.cpp"
#pragma warning(pop)

// LA CLAVE DE LA VISIBILIDAD
extern "C" {
    __declspec(dllexport) AEffect* VSTPluginMain(audioMasterCallback master) {
        Plugin* effect = new Plugin(master);
        if (!effect) return 0;
        return effect->getAeffect();
    }

    // Compatibilidad extra
    __declspec(dllexport) AEffect* main(audioMasterCallback master) {
        return VSTPluginMain(master);
    }
}
