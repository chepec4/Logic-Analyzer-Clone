#ifndef VERSION_INCLUDED
#define VERSION_INCLUDED

// -----------------------------------------------------------------------------
// 1. IDENTIDAD CORPORATIVA (PROTEGIDA)
// -----------------------------------------------------------------------------
#define NAME       "C4 Analyzer"
#define NAME_SHORT "ChepeC4"
#define COMPANY    "C4 Productions"
#define COPYRIGHT  "Copyright (c) 2026 C4 Productions"

// -----------------------------------------------------------------------------
// 2. VERSIÓN DEL SOFTWARE
// -----------------------------------------------------------------------------
// Definimos los números directamente para evitar errores de preprocesador en GCC
#define VER_MAJOR 1
#define VER_MINOR 0
#define VER_PATCH 0

// Versión numérica para cálculos internos del plugin
#define VERSION     1.00

// Versión en texto para mostrar en la interfaz (Acerca de...)
#define VERSION_STR "1.0.0"

// Versión formateada para los Recursos de Windows (.rc)
// Esto soluciona el error de "pasting" al compilar main.res
#define VERSION_RC  1,0,0,0

// -----------------------------------------------------------------------------
// NOTA TÉCNICA:
// Se han eliminado las macros recursivas (PP_CAT, PP_STR) que causaban
// el error "pasting tokens" en MinGW/GCC 12.
// -----------------------------------------------------------------------------

#endif // VERSION_INCLUDED
