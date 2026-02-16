#ifndef INCLUDES_INCLUDED
#define INCLUDES_INCLUDED

// 1. Plataforma Base (Debe ser el primero)
#include "kali/platform.h"

// ----------------------------------------------------------------------------
// [C4 FIX] Parches de Compatibilidad GCC/MinGW
// ----------------------------------------------------------------------------

// Fix para __PRETTY_FUNCTION__ como inicializador de array en Kali
#ifdef FUNCSIG_
#undef FUNCSIG_
#endif
#define FUNCSIG_ "Function"

// Fix para atributo de inlining forzado
#ifndef __forceinline
#define __forceinline inline __attribute__((always_inline))
#endif

// ----------------------------------------------------------------------------
// 2. Includes del Sistema Operativo
// ----------------------------------------------------------------------------

#if MACOSX_
    #import <Cocoa/Cocoa.h>
#endif

#if WINDOWS_
    // Limpieza de warnings de headers de Windows
    #ifndef __GNUC__
        #pragma warning(push, 3)
    #endif

    #ifndef _WIN32_WINNT
        #define _WIN32_WINNT 0x0501 // XP o superior
    #endif

    #define WIN32_LEAN_AND_MEAN
    #define NOMINMAX // Evita conflictos con std::min/max
    #include <windows.h>
    #include <commctrl.h>
    
    // OpenGL
    #include <GL/gl.h>

    // Parche legacy
    #undef small

    #ifndef __GNUC__
        #pragma warning(pop)
        // Desactivación de warnings específicos de MSVC si fuera necesario
        #pragma warning(disable: 4127) 
    #endif
#endif

// 3. Tipos Globales de Kali (Necesario para widgets.h)
namespace kali {
    typedef const char* Resource;
}

#endif // INCLUDES_INCLUDED
