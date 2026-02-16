#include "includes.h"
#include "main.h"
#include "kali/app.dll.h" // Implementa DllMain
#include <exception>

// ============================================================================
// VST 2.4 ENTRY POINT
// ============================================================================

// [C4 FIX] Definición explícita de exportación para DLL de Windows
#define VST_EXPORT extern "C" __declspec(dllexport)

// Función principal llamada por el Host (Cubase, FL Studio, etc.)
VST_EXPORT AEffect* VSTPluginMain(audioMasterCallback audioMaster)
{
    // Chequeo de fugas de memoria en Debug (MSVC)
    #ifdef _CRTDBG_MAP_ALLOC
        _CrtSetDbgFlag(_CRTDBG_CHECK_ALWAYS_DF | _CRTDBG_LEAK_CHECK_DF);
    #endif

    if (!audioMaster) return nullptr;

    // Handshake inicial con el Host
    // audioMasterVersion (2.4) debe estar definido en el SDK
    if (audioMaster(nullptr, audioMasterVersion, 0, 0, nullptr, 0.0f) == 0)
        return nullptr;

    // Inicialización del sistema de trazas
    #if defined(DBG) && (DBG > 0)
        try {
            ::trace.setLevel(::trace.Full);
        } catch(...) {}
    #endif

    // Creación segura del Plugin
    try {
        // 'Plugin' hereda de AudioEffectX y gestiona su propia memoria (AlignedNew)
        Plugin* plugin = new Plugin(audioMaster);
        
        // Obtener la estructura C para el host
        AEffect* effect = plugin->getAeffect();

        // Validación crítica
        if (!effect) {
            delete plugin;
            return nullptr;
        }
        
        return effect;
    }
    catch (const std::exception& e) {
        ::trace.full("[FATAL] Exception in VSTPluginMain: %s\n", e.what());
        return nullptr;
    }
    catch (...) {
        ::trace.full("[FATAL] Unknown exception in VSTPluginMain\n");
        return nullptr;
    }
}

// Alias legacy para hosts antiguos (VSTMain)
VST_EXPORT AEffect* main(audioMasterCallback audioMaster) {
    return VSTPluginMain(audioMaster);
}
