#include "includes.h"
#include "main.h"

/**
 * REGLA DE ORO:
 * No incluimos "kali/app.dll.h" directamente aquí si vamos a personalizar 
 * la inicialización, o bien, dejamos que Kali gestione el DllMain.
 * * Para este proyecto, el DllMain ya está definido en la librería Kali.
 * Por lo tanto, usamos un Hook de inicialización o delegamos la carga.
 */

// ............................................................................

/**
 * NOTA DE INGENIERÍA:
 * Se ha eliminado la redefinición de DllMain para evitar el error de 
 * "multiple definition". Las configuraciones de depuración se mueven 
 * al constructor del objeto global o a la fase de carga del Plugin.
 */

namespace {
    /**
     * Esta estructura estática se ejecuta al cargar la DLL en memoria.
     * Es la forma más limpia y compatible de configurar el entorno de Debug
     * sin chocar con el DllMain de las librerías base.
     */
    struct InitPlugin {
        InitPlugin() {
            #if DBG
                // Configuración de rastreo de fugas de memoria (Punto 1)
                #ifdef _CRTDBG_MAP_ALLOC
                    _CrtSetDbgFlag(_CRTDBG_CHECK_ALWAYS_DF | _CRTDBG_LEAK_CHECK_DF);
                #endif

                // Nivel de traza máximo para desarrollo
                #if DBG > 1
                    trace.setLevel(trace.Full);
                    trace.full("C4 Analyzer: Motor de depuración iniciado.\n");
                #endif
            #endif
        }
    } _init;
}

// ............................................................................

/**
 * REVISIÓN DE INTEGRIDAD:
 * 1. Se centraliza VSTPluginMain en 'vstsdk-wrapper.cpp'.
 * 2. La alineación SSE está garantizada por la herencia de sp::AlignedNew en main.h.
 * 3. La comunicación con el Master del DAW es manejada por el Wrapper.
 */

// ............................................................................
