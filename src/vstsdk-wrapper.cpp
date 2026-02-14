#include <windows.h>
#include <commdlg.h>
#include <objidl.h>
#include <shlwapi.h>

#include "main.h" 
#include "includes.h"

#pragma warning(push, 3)
#include "audioeffect.cpp"
#include "audioeffectx.cpp"
#pragma warning(pop)

extern "C" {
    // Solo exportamos VSTPluginMain
    __declspec(dllexport) AEffect* VSTPluginMain(audioMasterCallback master) {
        Plugin* effect = new Plugin(master);
        if (!effect) return 0;
        return effect->getAeffect();
    }
}
