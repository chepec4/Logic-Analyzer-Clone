#ifndef VERSION_INCLUDED
#define VERSION_INCLUDED

// 1. DEFINICIONES NUMÉRICAS (Para lógica interna y cálculos)
#define VERSION_MAJOR 1
#define VERSION_MINOR 0
#define VERSION_PATCH 9  // Sincronizado con el estado de desarrollo actual
#define VERSION_BUILD 0

// 2. COMPATIBILIDAD CON main.h
// Meticulosidad: main.h requiere VERSION como número real para: int(VERSION * 1000)
#ifndef VERSION
 #define VERSION 1.09
#endif

// 3. IDENTIDAD VISUAL Y DE SISTEMA
#ifndef NAME
 #define NAME "C4 Analyzer"
#endif

#ifndef COMPANY
 #define COMPANY "C4 Productions"
#endif

#ifndef COPYRIGHT
 #define COPYRIGHT "Copyright (c) 2026 C4 Productions"
#endif

// 4. CADENAS PARA RECURSOS DE WINDOWS (version.rc)
// REGLA DE ORO: VERSION_STR debe ser una cadena pura para la propiedad "FileVersion"
#define VERSION_STR "1.0.9"

// VERSION_RC debe usar comas para la estructura VS_FIXEDFILEINFO de Windows
#define VERSION_RC VERSION_MAJOR,VERSION_MINOR,VERSION_PATCH,VERSION_BUILD

// 5. HELPER MACROS (Para automatización de strings si fuera necesario)
#define PP_STR(A)          PP_STR__(A)
#define PP_STR__(A)        #A

#endif // VERSION_INCLUDED
