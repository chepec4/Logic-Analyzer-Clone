#ifndef KALI_PLATFORM_INCLUDED
#define KALI_PLATFORM_INCLUDED

// ............................................................................
// PLATFORM ABSTRACTION LAYER: macOS
// Este archivo define las primitivas del compilador para entornos Apple (Clang/GCC).
// ............................................................................

#define MACOSX_      1 // Identificador de Plataforma

// Control de Visibilidad de Símbolos (Dynamic Library Export)
#define API_EXPORT __attribute__ ((visibility("default")))

// Metadatos de Funciones (Reflexión estática)
#define FUNCTION_  __FUNCTION__
#define FUNCSIG_   __PRETTY_FUNCTION__

// Optimizaciones de Compilador
// [C4 OPTIMIZATION] Se añade 'inline' explícito y se usa __restrict__ estándar
#define inline_    __attribute__ ((always_inline)) inline
#define restrict_  __restrict__

// Validación de cadenas de formato tipo printf (Check en tiempo de compilación)
#define format__   __attribute__ ((format(printf, 2, 3)))

// ............................................................................

#endif // ~ KALI_PLATFORM_INCLUDED
