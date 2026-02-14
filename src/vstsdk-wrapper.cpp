#ifndef _DEBUG
#define _DEBUG 0
#endif

#include "main.h"
#include "includes.h"

// Forzamos la inclusión del código fuente del SDK
#pragma warning(push, 3)
#include "audioeffect.cpp"
#include "audioeffectx.cpp"
#pragma warning(pop)

extern "C" {
    // Definimos VSTPluginMain como la única entrada válida
    __declspec(dllexport) AEffect* VSTPluginMain(audioMasterCallback master) {
        Plugin* effect = new Plugin(master);
        if (!effect) return 0;
        return effect->getAeffect();
    }
}
