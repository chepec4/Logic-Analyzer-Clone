// IMPORTANTE: main.h debe ir primero
#include "main.h" 
#include "includes.h"

// Desactivamos warnings molestos del SDK
#pragma warning(push, 3)
#include "audioeffect.cpp"
#include "audioeffectx.cpp"
#pragma warning(pop)

extern "C" {
    // Puerta de entrada para el DAW
    __declspec(dllexport) AEffect* VSTPluginMain(audioMasterCallback master) {
        Plugin* effect = new Plugin(master);
        if (!effect) return 0;
        return effect->getAeffect();
    }
    
    // Alias legacy
    __declspec(dllexport) AEffect* main(audioMasterCallback master) {
        return VSTPluginMain(master);
    }
}
