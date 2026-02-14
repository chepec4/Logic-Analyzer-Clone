#include "includes.h"

// Forzamos la inclusión del código fuente del SDK de Steinberg
#pragma warning(push, 3)
#include "audioeffect.cpp"
#include "audioeffectx.cpp"
#pragma warning(pop)

// ----------------------------------------------------------------------------
// LA PUERTA DE ENTRADA (VST EXPORT)
// Esto es lo que Ableton y FL Studio buscan para reconocer el plugin.
// ----------------------------------------------------------------------------
extern "C" {
    // __declspec(dllexport) hace que la función sea visible fuera del DLL
    __declspec(dllexport) AEffect* VSTPluginMain(audioMasterCallback master) {
        // Creamos una instancia de tu clase Plugin definida en main.h
        Plugin* effect = new Plugin(master);
        if (!effect) return 0;
        return effect->getAeffect();
    }
    
    // Versión antigua por compatibilidad
    __declspec(dllexport) AEffect* main(audioMasterCallback master) {
        return VSTPluginMain(master);
    }
}
