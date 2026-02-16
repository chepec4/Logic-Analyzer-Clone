#include "includes.h"

/**
 * VST SDK WRAPPER — RECONSTRUCCIÓN ESTRUCTURAL (C4 DEFINITIVE)
 * Optimizado para: MinGW-w64 GCC 12.2 / C++17
 * * Este archivo compila la implementación base del VST SDK dentro del DLL.
 * Actúa como un "Firewall" de advertencias, silenciando las quejas del compilador
 * sobre el código legacy de Steinberg (que no podemos modificar).
 */

#if defined(__GNUC__) || defined(__clang__)
    #pragma GCC diagnostic push
    // Ignorar variables no usadas en el SDK
    #pragma GCC diagnostic ignored "-Wunused-parameter"
    #pragma GCC diagnostic ignored "-Wunused-variable"
    // Ignorar indexado con char (común en código antiguo)
    #pragma GCC diagnostic ignored "-Wchar-subscripts"
    // Ignorar deprecaciones propias del SDK
    #pragma GCC diagnostic ignored "-Wdeprecated-declarations"
    // Ignorar conversión de string constantes a char*
    #pragma GCC diagnostic ignored "-Wwrite-strings"
    // Ignorar conversiones implícitas de tipos
    #pragma GCC diagnostic ignored "-Wconversion"
    #pragma GCC diagnostic ignored "-Wsign-compare"
#endif

// ============================================================================
// INCLUSIÓN DEL SDK (Unity Build)
// ============================================================================
// Las rutas de búsqueda (-I) en el Makefile permiten esta inclusión directa.

#include "audioeffect.cpp"
#include "audioeffectx.cpp"

// ============================================================================

#if defined(__GNUC__) || defined(__clang__)
    #pragma GCC diagnostic pop
#endif
