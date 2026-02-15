#ifndef INCLUDES_INCLUDED
#define INCLUDES_INCLUDED

#include "kali/platform.h"

// ----------------------------------------------------------------------------
// [C4 FIX] CORRECCIÓN CRÍTICA DE FUNCSIG_
// El error "initializer fails to determine size of 'aux'" ocurre porque
// kali/platform.h define FUNCSIG_ como __PRETTY_FUNCTION__ (una variable en GCC).
// Kali intenta hacer 'char aux[] = FUNCSIG_;', lo cual requiere un TEXTO LITERAL.
// Aquí lo forzamos a ser un texto literal para que compile.
// ----------------------------------------------------------------------------
#ifdef FUNCSIG_
#undef FUNCSIG_
#endif
#define FUNCSIG_ "Function"

// ----------------------------------------------------------------------------
// [C4 FIX] CORRECCIÓN DE __forceinline
// GCC usa atributos para el inlining forzado.
// ----------------------------------------------------------------------------
#ifndef __forceinline
#define __forceinline inline __attribute__((always_inline))
#endif

// ............................................................................

#if MACOSX_

#import <Cocoa/Cocoa.h>

#endif

// ............................................................................

#if WINDOWS_

// [C4 FIX] Ignorar pragmas de MSVC si estamos en GCC para limpiar la consola
#ifndef __GNUC__
#pragma warning(push, 3)
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600 // [C4 FIX] Subido a Vista (0x0600) para mejor soporte MinGW
#endif

#define ISOLATION_AWARE_ENABLED 1

// [C4 FIX] Aseguramos definición de debug
#ifndef _DEBUG
#define _DEBUG 1
#endif

#if 0
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

// [C4 FIX] Inclusiones estándar requeridas antes de windows.h
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// [C4 FIX] Solución para el conflicto de min/max entre Windows y C++ estándar
#include <algorithm>
using std::min;
using std::max;

#include <windows.h>
#include <commctrl.h>
#include <GL/gl.h>

#undef small

// [C4 FIX] Parche de Tipos Kali
// Define 'Resource' aquí para evitar errores de 'typename' en widgets.h
namespace kali {
    typedef const char* Resource;
}

#ifndef __GNUC__
#pragma warning(pop)
#pragma warning(disable: 4127) // conditional expression is constant

// enable extra warnings:
#pragma warning(default: 4191) // unsafe conversion from 'type of expression' to 'type required'
#pragma warning(default: 4242) // (another) conversion from 'type1' to 'type2', possible loss of data
#pragma warning(default: 4254) // (another) conversion from 'type1' to 'type2', possible loss of data
#pragma warning(default: 4263) // member function does not override any base class virtual member function
#pragma warning(default: 4264) // no override available for virtual member function from base 'class'; function is hidden
#pragma warning(default: 4265) // class has virtual functions, but destructor is not virtual
#pragma warning(default: 4266) // no override available for virtual member function from base 'type'; function is hidden
#pragma warning(default: 4296) // expression is always false
#pragma warning(default: 4431) // missing type specifier - int assumed
#pragma warning(default: 4640) // construction of local static object is not thread-safe
#endif

#endif

// ............................................................................

#endif // INCLUDES_INCLUDED
