#include <windows.h>
#include "main.h" 
#include "includes.h"


#pragma warning(push, 3)
#include "audioeffect.cpp"
#include "audioeffectx.cpp"
#pragma warning(pop)

extern "C" {
 
    __declspec(dllexport) AEffect* VSTPluginMain(audioMasterCallback master) {
        
        if (!master(0, audioMasterVersion, 0, 0, 0, 0)) return 0;

      
        Plugin* effect = new Plugin(master);

        if (!effect) return 0;

     
        if (effect->getAeffect()->magic != kEffectMagic) {
            delete effect;
            return 0;
        }

        return effect->getAeffect();
    }
}
