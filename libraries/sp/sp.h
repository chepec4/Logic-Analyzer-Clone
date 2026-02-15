#ifndef SP_SP_INCLUDED
#define SP_SP_INCLUDED

#include "kali/platform.h"
#include <xmmintrin.h>
#include <emmintrin.h>
#include <math.h>

// Herencia de constantes base (pi, adn, etc.)
#include "sp/base.h"

// ============================================================================
// 1. MOTOR SIMD (m128 / m4f)
// ============================================================================

struct m128 {
    union {
        __m128 v;
        float  f[4];
    };

    typedef float Type;
    enum { size = 4 };

    __forceinline m128() {}
    __forceinline m128(__m128 i) : v(i) {}
    __forceinline m128(float i)  : v(_mm_set_ps1(i)) {}

    __forceinline operator __m128() const { return v; }

    __forceinline float& operator[](int i) { return f[i]; }
    __forceinline const float& operator[](int i) const { return f[i]; }
};

// Operadores Vectoriales (Optimización para gnu++14)
__forceinline m128 operator + (m128 a, m128 b) { return _mm_add_ps(a.v, b.v); }
__forceinline m128 operator - (m128 a, m128 b) { return _mm_sub_ps(a.v, b.v); }
__forceinline m128 operator * (m128 a, m128 b) { return _mm_mul_ps(a.v, b.v); }
__forceinline m128 operator / (m128 a, m128 b) { return _mm_div_ps(a.v, b.v); }

// Comparaciones y Matemáticas de Picos
__forceinline m128 min(m128 a, m128 b) { return _mm_min_ps(a.v, b.v); }
__forceinline m128 max(m128 a, m128 b) { return _mm_max_ps(a.v, b.v); }

template <int a, int b, int c, int d>
__forceinline m128 shuffle(m128 x, m128 y) {
    return _mm_shuffle_ps(x.v, y.v, _MM_SHUFFLE(d, c, b, a));
}

__forceinline m128 hsum(m128 x) {
    __m128 r = _mm_add_ps(x.v, _mm_movehl_ps(x.v, x.v));
    return _mm_add_ss(r, _mm_shuffle_ps(r, r, 1));
}

// ============================================================================
// 2. INFRAESTRUCTURA SP (Iteradores y Filtros)
// ============================================================================

namespace sp {
    typedef ::m128 m128;
    typedef ::m128 m4f;

    // [C4 FIX] Iterador de Miembros (Requerido por sa.display.h y analyzer.h)
    // Permite iterar sobre estructuras complejas accediendo solo a un campo.
    template <typename T, typename V, V T::*M>
    struct Iter {
        T* p;
        Iter(T* p) : p(p) {}
        __forceinline V& operator[](int i) const { return p[i].*M; }
    };

    // [C4 FIX] Iterador de Arreglos (Requerido por el banco de filtros)
    template <typename T, typename V>
    struct IterA {
        V* p;
        IterA(T* base, int offset = 0) {
            p = (V*)((char*)base + offset);
        }
        __forceinline V& operator[](int i) const { return p[i]; }
    };

    // Gestión de Memoria Alineada (Requerido por main.h)
    template <int Align>
    struct AlignedNew {
        void* operator new(size_t size) {
            return _mm_malloc(size, Align);
        }
        void operator delete(void* p) {
            _mm_free(p);
        }
    };

    // Estructura de Filtro 2-Pole (Topología optimizada para Analizador)
    struct TwoPoleLPSAx {
        enum { State = 2, Coeff = 3 };
        static inline_ m128 tick(float in, m4f* z, const m4f* k) {
            m128 out = z[0];
            z[0] = m128(in) * k[0] + z[0] * k[1] + z[1] * k[2];
            z[1] = out;
            return out;
        }
    };

} // ~ namespace sp

// Inclusiones de extensión
#include "sp/coefficients.h"
#include "sp/more.h"

#endif
