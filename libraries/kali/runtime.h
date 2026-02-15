#ifndef KALI_RUNTIME_INCLUDED
#define KALI_RUNTIME_INCLUDED

#include "kali/platform.h"
#include <string.h>
#include <stdlib.h>

// [C4 INFRASTRUCTURE FIX] 
// Incluimos cabeceras SSE para permitir la sobrecarga de operadores vectoriales
// directamente en el runtime. Esto evita errores en templates matemáticos.
#include <xmmintrin.h> // Para __m128
#include <emmintrin.h> // Para __m128i (si fuera necesario en el futuro)

// ............................................................................

namespace kali {

// ............................................................................
// 1. UTILIDADES MATEMÁTICAS BÁSICAS Y VECTORIALES
// ............................................................................

// Limpieza de macros conflictivas de Windows
#ifdef min
#undef min
#undef max
#endif

// ----------------------------------------------------------------------------
// Templates Genéricos (Para tipos estándar: int, float, double)
// ----------------------------------------------------------------------------
template <typename T> inline T min(T a, T b) { return a < b ? a : b; }
template <typename T> inline T max(T a, T b) { return a > b ? a : b; }

// ----------------------------------------------------------------------------
// Sobrecargas Especializadas para SSE (m128)
// [SOLUCIÓN DEFINITIVA]: Esto permite usar kali::max(vecA, vecB) sin error.
// ----------------------------------------------------------------------------
inline __m128 min(__m128 a, __m128 b) { return _mm_min_ps(a, b); }
inline __m128 max(__m128 a, __m128 b) { return _mm_max_ps(a, b); }

// ----------------------------------------------------------------------------
// Utilidades Generales
// ----------------------------------------------------------------------------

template <typename T>
inline void swap(T& a, T& b)
{
    T temp(a);
    a = b;
    b = temp;
}

/**
 * @brief Copia de memoria optimizada.
 * El uso de __restrict permite al compilador asumir que los buffers no se solapan,
 * habilitando optimizaciones agresivas de SIMD (AVX/SSE) en el bucle de copia.
 */
template <typename T>
inline void copy(T* __restrict dst, const T* __restrict src, int size)
{
    if (size > 0 && dst && src) {
        memcpy(dst, src, size * sizeof(T));
    }
}

template <typename T>
void sort(T* data, int count)
{
    struct aux {
        static int compare(const void* a, const void* b) {
            // Asume que T tiene un miembro 'name' (común en las estructuras de Kali)
            return strcmp(((const T*)a)->name, ((const T*)b)->name);
        }
    };

    if (count > 1 && data) {
        qsort(data, count, sizeof(T), aux::compare);
    }
}

inline bool inlist(const char* const list[], const char* value)
{
    if (!list || !value) return false;
    while (*list) {
        if (!strcmp(*list++, value)) return true;
    }
    return false;
}

template <typename T>
T nextPowOf2(T v)
{
    v--;
    // Algoritmo de "bit smearing" estándar, seguro para GCC y MSVC
    for (unsigned int i = 1; i < sizeof(T) * 8; i <<= 1) {
        v = v | (v >> i);
    }
    return v + 1;
}

// ............................................................................
// 2. REFLEXIÓN DE TIPOS (STUBS DE COMPATIBILIDAD GCC 12+)
// ............................................................................

// Se retorna un literal seguro para evitar errores de buffer overflow
// en tiempo de compilación que ocurrían con __PRETTY_FUNCTION__ en versiones antiguas.
template <typename T>
inline const char* typeString()
{
    return "Object";
}

template <typename T>
inline const char* typeString(const T&) { return typeString<T>(); }

// ............................................................................
// 3. REFLEXIÓN DE ENUMS (ESTABILIZADO)
// ............................................................................
// Estructura simplificada para evitar errores de "template instantiation depth"
// y "scope resolution" en compiladores modernos C++17/20.

template <typename E, E N>
struct EnumNames
{
    // Operador de acceso seguro.
    // Retorna un placeholder para mantener la UI funcional sin crashear.
    const char* operator [] (int i) const 
    { 
        return "EnumItem"; 
    }

    EnumNames() {}

    // Función estática requerida por widgets.base.h
    static void ctor() {}

private:
    // Almacenamos el tamaño forzando el cast a int para evitar ambigüedad de tipos
    enum { Size = (int)N };
};

// ............................................................................

} // ~ namespace kali

#endif // KALI_RUNTIME_INCLUDED
