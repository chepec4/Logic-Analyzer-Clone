#include "includes.h"
#include "main.h"
#include "kali/app.dll.h"
#include <exception>

/**
 * VST 2.4 ENTRY POINT — RECONSTRUCCIÓN ARQUITECTÓNICA (C4 DEFINITIVE)
 * Compatible con: GCC 12.2 (MinGW-w64), C++14, Windows Server 2025.
 * Resuelve: Conflicto de retorno de main(), visibilidad de trace y fugas de memoria.
 */

// ----------------------------------------------------------------------------
// EXPORTACIÓN DE SÍMBOLOS (Win64 ABI)
// ----------------------------------------------------------------------------

#define VST_EXPORT extern "C" __declspec(dllexport)

// Forward declaration para asegurar visibilidad
VST_EXPORT AEffect* VSTPluginMain(audioMasterCallback audioMaster);

// ----------------------------------------------------------------------------
// IMPLEMENTACIÓN PRINCIPAL
// ----------------------------------------------------------------------------

VST_EXPORT AEffect* VSTPluginMain(audioMasterCallback audioMaster)
{
    // 1. Gestión de Depuración (Win32 CRT)
    #ifdef _CRTDBG_MAP_ALLOC
        _CrtSetDbgFlag(_CRTDBG_CHECK_ALWAYS_DF | _CRTDBG_LEAK_CHECK_DF);
    #endif

    // 2. Validación de seguridad del Host
    if (audioMaster == nullptr) 
        return nullptr;

    // 3. Verificación de versión del SDK VST (2400 = VST 2.4)
    // El host debe responder positivamente a la consulta de versión.
    if (audioMaster(nullptr, audioMasterVersion, 0, 0, nullptr, 0.0f) == 0)
        return nullptr;

    // 4. Configuración del motor de traza de Kali
    // [C4 FIX] Se accede al objeto global 'trace' evitando la ambigüedad de kali::
    #if defined(DBG) && (DBG > 0)
        try {
            // El framework Kali define 'trace' como un objeto const global en dbgutils.h
            ::trace.setLevel(::trace.Full);
        } catch(...) {}
    #endif

    // 5. Instanciación Protegida del Plugin
    try
    {
        // NOTA: 'Plugin' debe estar completamente definido en main.h tras aplicar
        // el parche de sp::AlignedNew para evitar el error 'incomplete type'.
        Plugin* plugin = new Plugin(audioMaster);
        AEffect* effect = plugin->getAeffect();

        // Validación de integridad de la estructura VST
        if (effect == nullptr)
        {
            delete plugin;
            return nullptr;
        }

        return effect;
    }
    catch (const std::exception& e)
    {
        trace.warn("%s: %s\n", FUNCTION_, e.what());
        return nullptr;
    }
    catch (...)
    {
        trace.warn("%s: unknown exception\n", FUNCTION_);
        return nullptr;
    }
}

// ----------------------------------------------------------------------------
// COMPATIBILIDAD CON HOSTS LEGACY
// ----------------------------------------------------------------------------

/**
 * [C4 FIX] Resolución del error '::main must return int'.
 * En C++ moderno no podemos definir AEffect* main. 
 * Usamos una función puente que el enlazador (Linker) mapeará correctamente.
 */
VST_EXPORT AEffect* VSTMain(audioMasterCallback audioMaster)
{
    return VSTPluginMain(audioMaster);
}

/**
 * NOTA PARA EL LINKER:
 * Para máxima compatibilidad con hosts antiguos que buscan exactamente el símbolo "main",
 * el Makefile debe incluir la directiva: -Wl,--defsym,main=VSTPluginMain
 */
