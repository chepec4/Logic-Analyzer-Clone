#ifndef VERSION_INCLUDED
#define VERSION_INCLUDED

// ----------------------------------------------------------------------------
// IDENTIDAD CORPORATIVA C4
// ----------------------------------------------------------------------------
#define VER       1,0,0
#define NAME      "C4 Analyzer"         // Antes: Spectrum Analyzer
#define NAME_SHORT "C4Ana"              // Nombre corto para binarios
#define COMPANY   "C4 Designs"          // Antes: Seven Phases
#define COPYRIGHT "Copyright (c) 2026 C4 Designs"

// ----------------------------------------------------------------------------
// MACROS DEL SISTEMA (NO TOCAR)
// ----------------------------------------------------------------------------
#define PP_STR(A)          PP_STR__(A)
#define PP_STR_1(A)        PP_STR(A)
#define PP_STR__(A)        #A
#define PP_CAT(A, B)       PP_CAT__(A, B)
#define PP_CAT__(A, B)     A##B
#define VERSION__(A, B, C) A.PP_CAT(B, C)
#define VERSION            PP_CAT(VERSION__, (VER))
#define VERSION_STR        PP_STR_1(VERSION)
#define VERSION_RC         VER, 0

#endif // VERSION_INCLUDED
