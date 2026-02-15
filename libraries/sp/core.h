#ifndef SP_CORE_INCLUDED
#define SP_CORE_INCLUDED

#include <math.h>
#include <xmmintrin.h>
#include <malloc.h> // Necesario para _aligned_malloc en Windows
#include "kali/platform.h"

typedef __m128 m128;

// ............................................................................

namespace sp {

// ............................................................................

struct Undefined;
struct SSE;

/**
 * Template base para arrays genéricos.
 */
template <typename T, int N, typename Traits = Undefined>
struct array
{
    T value[N];

    enum     {size = N};
    typedef T Type;
    typedef T CppType[N];

    T  operator [] (int i) const {return value[i];}
    T& operator [] (int i)       {return value[i];}
};

/**
 * Especialización SSE para m4f (4 floats alineados).
 * REGLA DE ORO: Se añade unión para acceso directo a m128 sin penalización de memoria.
 */
template <>
struct array <float, 4, SSE>
{
    typedef float T;
    static const int N = 4;

    // Unión de ingeniería para paridad total entre C++ y Hardware SSE
    union {
        T    align_(16) value[N];
        m128 align_(16) v128;
    };

    enum     {size = N};
    typedef T Type;

    // Acceso a componentes individuales
    T  operator [] (int i) const {return value[i];}
    T& operator [] (int i)       {return value[i];}

    // Operadores de asignación y conversión implícita a m128
    // Esto permite usar m4f directamente en funciones de intrínsecos de Intel
    array& operator = (m128 v) { v128 = v; return *this; }
    operator m128 () const     { return v128; }

    // Constructor por defecto necesario para compatibilidad gnu++03
    array() { v128 = _mm_setzero_ps(); }
    
    // Constructor de copia SSE
    array(const array& other) { v128 = other.v128; }
    
    array& operator = (const array& v) {
        if (this != &v) v128 = v.v128;
        return *this;
    }
};

/**
 * Tipo fundamental para procesamiento vectorial en el analizador.
 */
typedef array <float, 4, SSE> m4f;

// ............................................................................

/**
 * Iterador para mapear estructuras de datos complejas a vectores SSE.
 */
template <typename T, typename U, U T::* value>
struct Iter
{
    T* a;
    typedef T Type;
    Iter(T* a) : a(a) {}
    U& operator [] (int i) {return a[i].*value;}
    const U& operator [] (int i) const {return a[i].*value;}
};

/**
 * Iterador para acceso a sub-elementos de un vector m4f dentro de un array.
 */
template <typename T, typename U>
struct IterA 
{
    T* a;
    int j;
    typedef U Type;
    IterA(T* a, int j) : a(a), j(j) {}
    U& operator [] (int i) const {return a[i][j];}
};

// ............................................................................

/**
 * Gestor de memoria alineada. 
 * CRÍTICO: Sin esto, el plugin colapsará (Crash) al usar instrucciones SSE en el heap.
 */
template <size_t A>
struct AlignedNew
{
    static void* operator new(size_t size) {
        void* ptr = _aligned_malloc(size, A);
        if (!ptr) throw; // Opcional: manejar error de memoria
        return ptr;
    }
    static void operator delete(void* ptr) {
        if (ptr) _aligned_free(ptr);
    }

protected:
    ~AlignedNew() {}
};

// ............................................................................

} // ~ namespace sp

// ............................................................................

#endif // ~ SP_CORE_INCLUDED
