#include "includes.h"
#include "main.h"
#include "kali/app.dll.h"


#ifdef WINDOWS_
extern "C" BOOL WINAPI DllMain(HINSTANCE hInst, DWORD dwReason, LPVOID /*lpvReserved*/)
{
    if (dwReason == DLL_PROCESS_ATTACH)
    {
        // Inicializamos los flags de depuración si estamos en modo Debug
        #ifdef _CRTDBG_MAP_ALLOC
            _CrtSetDbgFlag(_CRTDBG_CHECK_ALWAYS_DF | _CRTDBG_LEAK_CHECK_DF);
        #endif

        #if DBG > 1
            trace.setLevel(trace.Full);
        #endif

        // Registramos el módulo en el framework Kali
        kali::app->details_->module_ = hInst;
    }
    return TRUE;
}
#endif

// ............................................................................

/**
 * NOTA DE INGENIERÍA:
 * Hemos eliminado la función VSTPluginMain de este archivo para evitar 
 * conflictos con 'vstsdk-wrapper.cpp'. 
 * * La comunicación se centraliza en el wrapper para garantizar:
 * 1. Verificación de versión del Master.
 * 2. Alineación SSE (sp::AlignedNew).
 */

// ............................................................................
