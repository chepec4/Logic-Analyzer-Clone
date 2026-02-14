#include "main.h" // ¡FUNDAMENTAL! Aquí es donde vive la clase Plugin
#include "includes.h"

// Forzamos la inclusión del código fuente del SDK de Steinberg
#pragma warning(push, 3)
#include "audioeffect.cpp"
#include "audioeffectx.cpp"
#pragma warning(pop)

// ----------------------------------------------------------------------------
// LA PUERTA DE ENTRADA (VST EXPORT)
// ----------------------------------------------------------------------------
extern "C" {
    // Esta es la función que los DAWs de 64 bits buscan primero
    __declspec(dllexport) AEffect* VSTPluginMain(audioMasterCallback master) {
        // Ahora el compilador ya sabe qué es 'Plugin' gracias a main.h
        Plugin* effect = new Plugin(master);
        if (!effect) return 0;
        return effect->getAeffect();
    }
    
    // Cambiamos el nombre para evitar el error 'main must return int'
    // Los DAWs modernos ya usan VSTPluginMain, así que esto es por legacy.
    __declspec(dllexport) AEffect* VSTMain(audioMasterCallback master) {
        return VSTPluginMain(master);
    }
}
