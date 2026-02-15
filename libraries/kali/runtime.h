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

// Marcamos como inline para evitar conflictos con implementaciones de sp:: o std::
template <typename T> inline T min(T a, T b) { return a < b ? a : b; }
template <typename T> inline T max(T a, T b) { return a > b ? a : b; }

template <typename T>
inline void swap(T& a, T& b)
{
    T temp(a);
    a = b;
    b = temp;
}

/**
 * @brief Copia de memoria optimizada para buffers de audio.
 * Se utiliza __restrict para informar al compilador que los punteros no se solapan,
 * permitiendo vectorización SIMD automática en GCC.
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
    // Casting a unsigned int para asegurar que el shift de bits sea seguro en GCC
    for (unsigned int i = 1; i < sizeof(T) * 8; i <<= 1) {
        v = v | (v >> i);
    }
    return v + 1;
}

// ............................................................................
// 2. REFLEXIÓN DE TIPOS (STUBS DE COMPATIBILIDAD)
// ............................................................................
// Se elimina el parsing de FUNCSIG_ que causaba errores de tamaño en arrays 
// estáticos al no ser constantes literales en GCC.

template <typename T>
inline const char* typeString()
{
    return "Object";
}

template <typename T>
inline const char* typeString(const T&) { return typeString<T>(); }

// ............................................................................
// 3. REFLEXIÓN DE ENUMS (SANEAMIENTO DE RECURSIVIDAD)
// ............................................................................
// Esta estructura ha sido aplanada. Se eliminan las especializaciones de templates
// dentro de la struct para evitar el error de "non-namespace scope".

template <typename E, E N>
struct EnumNames
{
    // Constructor de acceso seguro. Devuelve un marcador de posición.
    // Esto previene que el compilador intente instanciar miles de plantillas.
    const char* operator [] (int i) const 
    { 
        return "EnumItem"; 
    }

    EnumNames() {}

    // Función estática requerida por la arquitectura original de Kali
    static void ctor() {}

private:
    // Almacenamos el tamaño como una constante entera simple.
    enum { Size = (int)N };
};

// ............................................................................

} // ~ namespace kali

#endif // KALI_RUNTIME_INCLUDED
