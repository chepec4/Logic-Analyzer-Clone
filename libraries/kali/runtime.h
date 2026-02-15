#ifndef KALI_RUNTIME_INCLUDED
#define KALI_RUNTIME_INCLUDED

#include "kali/platform.h"
#include <string.h>
#include <stdlib.h>

// ............................................................................

namespace kali {

// ............................................................................
// 1. UTILIDADES MATEMÁTICAS BÁSICAS
// ............................................................................

#ifdef min
#undef min
#undef max
#endif

template <typename T> inline T min(T a, T b) {return a < b ? a : b;}
template <typename T> inline T max(T a, T b) {return a > b ? a : b;}

template <typename T>
inline void swap(T& a, T& b)
{
    T temp(a);
    a = b;
    b = temp;
}

template <typename T>
inline void copy(T* restrict_ dst, const T* restrict_ src, int size)
{
    if (size > 0) memcpy(dst, src, size * sizeof(T));
}

template <typename T>
void sort(T* data, int count)
{
    struct aux {
        static int compare(const void* a, const void* b)
            {return strcmp(((const T*) a)->name, ((const T*) b)->name);}};

    if (count > 1) qsort(data, count, sizeof(T), aux::compare);
}

inline bool inlist(const char* const list[], const char* value)
{
    while (list && *list)
        if (!strcmp(*list++, value))
            return true;
    return false;
}

template <typename T>
T nextPowOf2(T v)
{
    v--;
    for (unsigned int i = 1; i < sizeof(T) * 8; i <<= 1)
        v = v | v >> i;
    return v + 1;
}

// ............................................................................
// 2. REFLEXIÓN DE TIPOS (STUBS PARA COMPATIBILIDAD GCC)
// ............................................................................
// Se ha simplificado para evitar errores de inicialización de arrays y
// recursividad infinita en MinGW/GCC.

template <typename T>
inline const char* typeString()
{
    // Devolvemos un genérico para evitar parsing complejo de firmas en GCC
    return "Object";
}

template <typename T>
inline const char* typeString(const T&) {return typeString<T>();}

// ............................................................................
// 3. REFLEXIÓN DE ENUMS (FIX CRÍTICO)
// ............................................................................
// Hemos eliminado la recursividad de plantillas que causaba el error
// "template instantiation depth exceeds maximum".

template <typename E, E N>
struct EnumNames
{
    // Retornamos un marcador de posición para evitar el crash del compilador.
    // En un VST, los nombres de los Enums solo se usan para depuración.
    const char* operator [] (int i) const { return "Value"; }

    EnumNames() {}

    // Función vacía para satisfacer las llamadas internas del código original
    static void ctor() {}

private:
    // Estructura simplificada sin recursión
    enum {Size = (int)N};
};

// ............................................................................

} // ~ namespace kali

#endif // KALI_RUNTIME_INCLUDED
