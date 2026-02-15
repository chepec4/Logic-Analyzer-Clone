#include "includes.h"
#include "main.h"
#include "kali/app.dll.h"

// ............................................................................
// VST ENTRY POINT
// ............................................................................
// Este es el punto de contacto entre el DAW (Host) y el C4 Analyzer.
// ............................................................................

extern "C" AEffect* VSTPluginMain(audioMasterCallback audioMaster)
{
    // Inicialización de herramientas de depuración de memoria (Solo MSVC)
    #ifdef _CRTDBG_MAP_ALLOC
        _CrtSetDbgFlag(_CRTDBG_CHECK_ALWAYS_DF | _CRTDBG_LEAK_CHECK_DF);
    #endif

    // Configuración del nivel de trazado/logs
    #if defined(DBG) && (DBG > 1)
        trace.setLevel(trace.Full);
    #endif

    // Verificación de la versión del Master del Host (DAW)
    if (!audioMaster) {
        return 0;
    }

    // audioMasterVersion suele ser 2400 para VST 2.4
    if (audioMaster(0, audioMasterVersion, 0, 0, 0, 0))
    {
        // Instanciación del Plugin. 
        // Gracias al cierre de namespaces en el Paso 4, 'Plugin' ahora es visible.
        Plugin* plugin = new Plugin(audioMaster);
        
        if (plugin) {
            return plugin->getAeffect();
        }
    }

    // Si llegamos aquí, la inicialización falló
    trace("%s: failed, wrong master version or allocation error.\n", FUNCTION_);
    return 0;
}

// ............................................................................
