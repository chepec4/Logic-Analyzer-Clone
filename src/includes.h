#ifndef INCLUDES_INCLUDED
#define INCLUDES_INCLUDED

// ============================================================================
// PARCHE DE COMPATIBILIDAD C4 ANALYZER (MSVC -> GCC/MinGW)
// ============================================================================

// 1. DEFINICIONES DE PLATAFORMA
// ----------------------------------------------------------------------------
// Aseguramos que el sistema sepa que estamos en Windows
#ifndef WINDOWS_
    #define WINDOWS_ 1
#endif

// 2. TRADUCCIÓN DE MACROS DE MICROSOFT
// ----------------------------------------------------------------------------
// GCC no tiene __FUNCSIG__, usamos __PRETTY_FUNCTION__ que es el equivalente.
// Soluciona error: '__FUNCSIG__' was not declared in this scope
#ifndef __FUNCSIG__
    #define __FUNCSIG__ __PRETTY_FUNCTION__
#endif

// MSVC usa __forceinline, GCC usa atributos.
#ifndef __forceinline
    #define __forceinline inline __attribute__((always_inline))
#endif

// 3. ENTORNO DE DEPURACIÓN
// ----------------------------------------------------------------------------
// Si no está definido NDEBUG (Release), asumimos modo Debug.
// Esto activa los rastreos (trace) en kali/dbgutils.h
#if !defined(NDEBUG) && !defined(_DEBUG)
    #define _DEBUG 1
#endif

// 4. INCLUSIÓN DE LIBRERÍAS DEL SISTEMA (Orden Crítico)
// ----------------------------------------------------------------------------
// Windows.h debe ir primero para definir tipos como HWND, DWORD, etc.
#include <windows.h>

// Librerías estándar de C/C++ necesarias para printf, math, etc.
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <string.h>

// 5. SOLUCIÓN AL CONFLICTO MIN/MAX
// ----------------------------------------------------------------------------
// El código antiguo usa min() y max() como macros o funciones indistintamente.
// Incluimos <algorithm> y forzamos el uso del estándar para evitar
// el error: "no matching function for call to min/max"
#include <algorithm>
using std::min;
using std::max;

// 6. PARCHES ESPECÍFICOS PARA LIBRERÍA 'KALI'
// ----------------------------------------------------------------------------
// Kali usa un tipo 'Resource' que a veces no se resuelve bien en GCC
// sin una definición explícita previa.
namespace kali {
    typedef const char* Resource;
}

// ============================================================================
// FIN DEL PARCHE C4
// ============================================================================

// A continuación, el código original o inclusiones adicionales del proyecto
// (Si el archivo original tenía más includes abajo, el compilador los leerá)

#pragma warning(disable: 4996) // Silencia warnings de funciones "inseguras" (sprintf, etc.)

#endif // INCLUDES_INCLUDED
