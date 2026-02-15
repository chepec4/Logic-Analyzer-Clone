#include "includes.h"
#include "main.h"
#include "kali/app.dll.h"

// ............................................................................
// VST ENTRY POINT DEFINITION
// ............................................................................
// Forzamos la visibilidad global para que el Host (DAW) encuentre la función.
// ............................................................................

extern "C" {
    // Algunos Hosts antiguos buscan 'main' en lugar de 'VSTPluginMain'
    #define VST_ENTRY VSTPluginMain
    
    AEffect* VST_ENTRY(audioMasterCallback audioMaster)
    {
        // 1. Gestión de Debug (Solo modo desarrollo MSVC)
        #ifdef _CRTDBG_MAP_ALLOC
            _CrtSetDbgFlag(_CRTDBG_CHECK_ALWAYS_DF | _CRTDBG_LEAK_CHECK_DF);
        #endif

        // 2. Configuración de Trazado de Kali
        // Usamos el namespace explícito para evitar 'not declared' en entornos GCC
        #if defined(DBG) && (DBG > 0)
            kali::trace.setLevel(kali::trace.Full);
        #endif

        // 3. Validación de Seguridad del Host
        if (!audioMaster) {
            return 0;
        }

        // 4. Verificación de Versión del SDK VST (2400 = VST 2.4)
        // El primer argumento 0 solicita la versión al Host.
        if (audioMaster(0, audioMasterVersion, 0, 0, 0, 0) == 0) {
            // Si el host devuelve 0, no es compatible con esta versión del SDK
            return 0;
        }

        try {
            // 5. Instanciación del Plugin
            // Tras reparar 'sp.h' y 'widgets.h', Plugin ya es un tipo completo.
            Plugin* plugin = new Plugin(audioMaster);
            
            if (plugin) {
                return plugin->getAeffect();
            }
        }
        catch (...) {
            // Protección contra fallos críticos en la carga de memoria o recursos
            return 0;
        }

        return 0;
    }
}

// ............................................................................
// NOTA TÉCNICA:
// El archivo kali/app.dll.h incluido al inicio gestiona automáticamente 
// el DllMain necesario para Windows, asociando la instancia del DLL 
// con el objeto 'kali::app'.
// ............................................................................
