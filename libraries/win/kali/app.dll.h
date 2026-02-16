#ifndef KALI_APP_DLL_INCLUDED
#define KALI_APP_DLL_INCLUDED

// Este archivo se incluye en main.cpp para definir el Entry Point de la DLL.
// Solo se activa si se compila como DLL.

#if defined(_DLL) || defined(_WINDLL) || defined(WIN32)

// ............................................................................

extern "C" BOOL WINAPI DllMain(HMODULE module, DWORD reason, LPVOID)
{
    using namespace kali;

    switch (reason)
    {
        case DLL_PROCESS_ATTACH:
            // Inicialización del Singleton de la App
            app.alloc();
            // Inicialización de detalles internos
            app->details_ = app->autorelease(new AppDetails);
            app->details_->module_ = module;
            
            // Opcional: DisableThreadLibraryCalls(module) para optimizar
            break;

        case DLL_PROCESS_DETACH:
            // Limpieza ordenada
            app.release();
            break;
    }

    return TRUE;
}

// ............................................................................

#endif 
#endif // ~ KALI_APP_DLL_INCLUDED
