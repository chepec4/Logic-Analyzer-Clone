#ifndef INCLUDES_INCLUDED
#define INCLUDES_INCLUDED

#include "kali/platform.h"

// ............................................................................

#if MACOSX_
    // Soporte para entornos Apple (si se portara el proyecto)
    #import <Cocoa/Cocoa.h>
#endif

// ............................................................................

#if WINDOWS_

#pragma warning(push, 3)

// Aseguramos compatibilidad mínima con Windows XP SP2 (Requisito de WDK 7.1)
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif

// Activamos soporte para temas visuales modernos (Visual Styles)
#define ISOLATION_AWARE_ENABLED 1

/**
 * DEPURACIÓN DE MEMORIA:
 * Si se activa DBG en el makefile, habilitamos el rastreador de fugas.
 */
#if DBG
    #ifndef _CRTDBG_MAP_ALLOC
        #define _CRTDBG_MAP_ALLOC
    #endif
    #include <stdlib.h>
    #include <crtdbg.h>
#endif

#include <math.h>
#include <windows.h>
#include <commctrl.h>
#include <gl/gl.h>      // Core de OpenGL para sa.display.h
#include <gl/glu.h>     // Utilidades de OpenGL

// Limpieza de macros problemáticas de Windows
#undef small
#undef min
#undef max

#pragma warning(pop)

// --- CONFIGURACIÓN DE ADVERTENCIAS METICULOSA ---
#pragma warning(disable: 4127) // Expresión condicional constante (común en templates)
#pragma warning(disable: 4512) // No se pudo generar el operador de asignación
#pragma warning(disable: 4201) // Extensiones no estándar: unión/estructuras anónimas (Crítico para core.h)

// Habilitar advertencias de seguridad extra para el código de audio
#pragma warning(default: 4191) // Conversión insegura de tipos
#pragma warning(default: 4242) // Posible pérdida de datos en conversión
#pragma warning(default: 4265) // Clase con funciones virtuales pero destructor no virtual
#pragma warning(default: 4640) // Construcción de objeto estático local no segura para hilos

#endif

// ............................................................................

#endif // INCLUDES_INCLUDED
