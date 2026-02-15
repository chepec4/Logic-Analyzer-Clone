#include "includes.h"
#include "main.h"
#include "kali/app.dll.h"

/**
 * MOTOR DE ENTRADA VST (ENTRY POINT)
 * Optimizado para MinGW x64 y compatibilidad con estándar gnu++14.
 * Respetando la infraestructura de Seven Phases / Kali.
 */

extern "C" {

    // Exportación explícita para el Host (DAW)
    #define VST_EXPORT __attribute__((visibility("default")))

    VST_EXPORT AEffect* VSTPluginMain(audioMasterCallback audioMaster)
    {
        // 1. Inicialización de Debug (Win32 CRT)
        #ifdef _CRTDBG_MAP_ALLOC
            _CrtSetDbgFlag(_CRTDBG_CHECK_ALWAYS_DF | _CRTDBG_LEAK_CHECK_DF);
        #endif

        // 2. Validación inicial del Host
        if (!audioMaster) return 0;

        // 3. Verificación de versión del SDK VST (Requerido por VST 2.4)
        if (!audioMaster(0, audioMasterVersion, 0, 0, 0, 0)) return 0;

        // 4. Configuración del motor de traza de Kali
        // [C4 FIX] Usamos una referencia local para evitar ambigüedad de namespace en GCC 12
        #if defined(DBG) && (DBG > 0)
            try {
                // Acceso seguro a la instancia global definida en dbgutils.h
                kali::trace.setLevel(kali::trace.Full);
            } catch(...) {}
        #endif

        // 5. Instanciación Protegida del Plugin
        try {
            // NOTA: Para que esta línea funcione, 'main.h' DEBE compilarse 
            // correctamente tras aplicar el parche de 'sp::AlignedNew' en 'sp.h'.
            Plugin* plugin = new Plugin(audioMaster);
            
            if (plugin) {
                return plugin->getAeffect();
            }
        }
        catch (...) {
            // Silenciamos fallos críticos para evitar que el DAW se cierre (crash)
            return 0;
        }

        return 0;
    }

    // Compatibilidad con Hosts extremadamente antiguos (buscan "main")
    VST_EXPORT AEffect* main(audioMasterCallback audioMaster) {
        return VSTPluginMain(audioMaster);
    }
}

/**
 * DOCUMENTACIÓN DE INFRAESTRUCTURA:
 * - 'kali/app.dll.h' inyecta el DllMain necesario para Windows.
 * - La clase 'Plugin' debe estar definida totalmente en 'main.h'.
 * - El uso de 'audioMaster' es el único canal de comunicación Host-Plugin.
 */
