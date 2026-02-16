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
// Definimos los números directamente para evitar errores de "token pasting"
#define VER_MAJOR 1
#define VER_MINOR 1
#define VER_PATCH 0

// Versión numérica (float) para cálculos internos del plugin
#define VERSION     1.10

// Versión en texto para mostrar en la interfaz (Acerca de...)
#define VERSION_STR "1.1.0"

// -----------------------------------------------------------------------------
// NOTA TÉCNICA:
// El archivo main.rc tiene sus propias definiciones para evitar conflictos
// de inclusión con el compilador de recursos (windres).
// Mantener ambos archivos sincronizados manualmente si se cambia la versión.
// -----------------------------------------------------------------------------

#endif // VERSION_INCLUDED
