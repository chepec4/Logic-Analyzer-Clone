#include "main.h"
#include "includes.h"

#pragma warning(push, 3)
#include "audioeffect.cpp"
#include "audioeffectx.cpp"
#pragma warning(pop)

// Esta función es el estándar que FL Studio busca para cargar el plugin
extern "C" {
    __declspec(dllexport) AEffect* VSTPluginMain(audioMasterCallback master) {
        Plugin* effect = new Plugin(master);
        if (!effect) return 0;
        return effect->getAeffect();
    }
    
    // Alias por si el DAW es muy antiguo
    __declspec(dllexport) AEffect* main(audioMasterCallback master) {
        return VSTPluginMain(master);
    }
}
