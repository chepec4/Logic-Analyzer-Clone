/**
 * VST SDK UNITY BUILD WRAPPER
 * * Este archivo compila la implementación del SDK de Steinberg.
 * Permite aislar los warnings del código legacy (C++98) del resto del proyecto.
 */

// Desactivar warnings de seguridad de CRT para el código antiguo de Steinberg
#define _CRT_SECURE_NO_WARNINGS

// Silenciar warnings específicos de GCC/MinGW para el SDK
#if defined(__GNUC__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wwrite-strings"
    #pragma GCC diagnostic ignored "-Wconversion"
    #pragma GCC diagnostic ignored "-Wunused-parameter"
    #pragma GCC diagnostic ignored "-Wcast-qual"
#endif

// Inclusión de la implementación del SDK (Unity Build)
// El makefile ya tiene los -I correctos para encontrar estos archivos
#include "audioeffect.cpp"
#include "audioeffectx.cpp"

#if defined(__GNUC__)
    #pragma GCC diagnostic pop
#endif
