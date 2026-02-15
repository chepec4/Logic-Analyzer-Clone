#include "includes.h"
#include "main.h"
#include "kali/app.dll.h"
#include <exception>

/**
 * VST 2.4 ENTRY POINT — RECONSTRUCCIÓN ARQUITECTÓNICA (C4 DEFINITIVE)
 * Versión Hardened: Captura de excepciones y protección de punteros.
 */

#define VST_EXPORT extern "C" __declspec(dllexport)

VST_EXPORT AEffect* VSTPluginMain(audioMasterCallback audioMaster)
{
    #ifdef _CRTDBG_MAP_ALLOC
        _CrtSetDbgFlag(_CRTDBG_CHECK_ALWAYS_DF | _CRTDBG_LEAK_CHECK_DF);
    #endif

    if (audioMaster == nullptr) return nullptr;

    if (audioMaster(nullptr, audioMasterVersion, 0, 0, nullptr, 0.0f) == 0)
        return nullptr;

    #if defined(DBG) && (DBG > 0)
        try {
            // Acceso al objeto global ::trace de kali/dbgutils.h
            ::trace.setLevel(::trace.Full);
        } catch(...) {}
    #endif

    try {
        // La clase Plugin debe estar completa tras el fix de sp::AlignedNew
        Plugin* plugin = new Plugin(audioMaster);
        AEffect* effect = plugin->getAeffect();

        if (effect == nullptr) {
            delete plugin;
            return nullptr;
        }
        return effect;
    }
    catch (const std::exception& e) {
        // Log de error específico si el framework lo soporta
        ::trace.full("Exception in VSTPluginMain: %s\n", e.what());
        return nullptr;
    }
    catch (...) {
        return nullptr;
    }
}

// Compatibilidad VST Legacy
VST_EXPORT AEffect* VSTMain(audioMasterCallback audioMaster) {
    return VSTPluginMain(audioMaster);
}

/**
 * [C4 FIX] El DAW busca 'VSTPluginMain' en x64. 
 * 'VSTMain' se incluye como alias para hosts antiguos.
 */
