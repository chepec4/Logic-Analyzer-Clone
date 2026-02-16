#ifndef KALI_RUNTIME_INCLUDED
#define KALI_RUNTIME_INCLUDED

#include "kali/platform.h"
#include <string.h>
#include <stdlib.h>
#include <new>

// Soporte SIMD para sobrecargas matemáticas
#include <xmmintrin.h>
#include <emmintrin.h>

// Limpieza de macros de Windows
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

namespace kali {

// ............................................................................
// Math Utilities & SIMD Overloads
// ............................................................................

template <typename T> inline T min(T a, T b) { return a < b ? a : b; }
template <typename T> inline T max(T a, T b) { return a > b ? a : b; }

// SSE Overloads
inline __m128 min(__m128 a, __m128 b) { return _mm_min_ps(a, b); }
inline __m128 max(__m128 a, __m128 b) { return _mm_max_ps(a, b); }

// ............................................................................
// General Utilities
// ............................................................................

template <typename T>
inline void swap(T& a, T& b)
{
    T temp(a);
    a = b;
    b = temp;
}

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
    for (unsigned int i = 1; i < sizeof(T) * 8; i <<= 1) {
        v = v | (v >> i);
    }
    return v + 1;
}

// ............................................................................
// Reflection Stubs (GCC 12 Compatibility)
// ............................................................................

template <typename T>
inline const char* typeString() { return "Object"; }

template <typename T>
inline const char* typeString(const T&) { return typeString<T>(); }

template <typename E, E N>
struct EnumNames
{
    const char* operator [] (int i) const { return "EnumItem"; }
    EnumNames() {}
    static void ctor() {}
private:
    enum { Size = (int)N };
};

} // ~ namespace kali

#endif // KALI_RUNTIME_INCLUDED
