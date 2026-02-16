#include "includes.h"

/**
 * VST SDK WRAPPER — RECONSTRUCCIÓN ESTRUCTURAL (C4 DEFINITIVE)
 * Optimizado para: MinGW-w64 GCC 12.2 / C++17
 * * Este archivo compila la implementación base del VST SDK. 
 * Se utilizan pragmas de GCC para ignorar advertencias de un SDK legacy
 * que no cumple con los estándares modernos de C++.
 */

#if defined(__GNUC__) || defined(__clang__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wunused-parameter"
    #pragma GCC diagnostic ignored "-Wunused-variable"
    #pragma GCC diagnostic ignored "-Wchar-subscripts"
    #pragma GCC diagnostic ignored "-Wdeprecated-declarations"
    #pragma GCC diagnostic ignored "-Wwrite-strings"
#endif

// Inclusión de la implementación del SDK de Steinberg
// Las rutas están resueltas por los flags -I del Makefile
#include "audioeffect.cpp"
#include "audioeffectx.cpp"

#if defined(__GNUC__) || defined(__clang__)
    #pragma GCC diagnostic pop
#endif

/**
 * JUSTIFICACIÓN ARQUITECTÓNICA:
 * Al incluir los archivos .cpp del SDK aquí, consolidamos la lógica del host
 * en una sola unidad de traducción. Esto permite al compilador aplicar
 * optimizaciones de inter-procedimiento (IPO) que son vitales para reducir
 * el overhead de comunicación entre el DAW y el C4 Analyzer.
 */
