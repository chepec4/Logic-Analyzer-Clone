#ifndef SP_SP_INCLUDED
#define SP_SP_INCLUDED

#include "kali/platform.h"
#include <xmmintrin.h> // SSE
#include <emmintrin.h> // SSE2
#include <math.h>

// Incluimos la base primero para heredar constantes como PI, adn y g2dB
#include "sp/base.h"

// ============================================================================
// 1. WRAPPER SSE GLOBAL
// ============================================================================
// Estructura fundamental para el procesamiento vectorial de audio.

struct m128
{
    union {
        __m128 v;    // Registro SIMD nativo
        float  f[4]; // Acceso escalar
    };

    typedef float Type;
    enum { size = 4 };

    __forceinline m128() {}
    __forceinline m128(__m128 i) : v(i) {}
    __forceinline m128(float i)  : v(_mm_set_ps1(i)) {}

    // Conversión automática a registro nativo para ser usado en intrínsecos _mm_...
    __forceinline operator __m128() const { return v; }

    // Acceso directo a componentes (Fix para k[i][0])
    __forceinline float& operator[](int i) { return f[i]; }
    __forceinline const float& operator[](int i) const { return f[i]; }
};

// ============================================================================
// 2. OPERADORES SIMD GLOBALES
// ============================================================================

__forceinline m128 operator + (const m128& a, const m128& b) { return _mm_add_ps(a.v, b.v); }
__forceinline m128 operator - (const m128& a, const m128& b) { return _mm_sub_ps(a.v, b.v); }
__forceinline m128 operator * (const m128& a, const m128& b) { return _mm_mul_ps(a.v, b.v); }
__forceinline m128 operator / (const m128& a, const m128& b) { return _mm_div_ps(a.v, b.v); }

// Comparaciones vectoriales requeridas por el motor de picos
__forceinline m128 operator == (const m128& a, const m128& b) { return _mm_cmpeq_ps(a.v, b.v); }
__forceinline m128 operator <  (const m128& a, const m128& b) { return _mm_cmplt_ps(a.v, b.v); }
__forceinline m128 operator >  (const m128& a, const m128& b) { return _mm_cmpgt_ps(a.v, b.v); }

__forceinline m128 min(const m128& a, const m128& b) { return _mm_min_ps(a.v, b.v); }
__forceinline m128 max(const m128& a, const m128& b) { return _mm_max_ps(a.v, b.v); }

// Plantilla shuffle para interband() y CardinalSpline
template <int a, int b, int c, int d>
__forceinline m128 shuffle(const m128& x, const m128& y) {
    return _mm_shuffle_ps(x.v, y.v, _MM_SHUFFLE(d, c, b, a));
}

// Suma horizontal (H-Sum). Devuelve m128 para compatibilidad con _mm_store_ss
__forceinline m128 hsum(const m128& x) {
    __m128 r = _mm_add_ps(x.v, _mm_movehl_ps(x.v, x.v));
    return _mm_add_ss(r, _mm_shuffle_ps(r, r, 1));
}

// ============================================================================
// 3. NAMESPACE SP (Infraestructura de Audio)
// ============================================================================

namespace sp {

    typedef ::m128 m128;
    typedef ::m128 m4f;

    // IterA: Adaptador de iteración para el motor del analizador.
    // [C4 FIX] Añadido constructor de 2 argumentos para analyzer.h:232
    template <typename T, typename V>
    struct IterA {
        V* ptr;
        IterA(T* base) : ptr((V*)base) {}
        IterA(T* base, int offset) : ptr(&((V*)base)[offset]) {}
        __forceinline V& operator[](int i) { return ptr[i]; }
    };

    // Estructuras de Filtros (Topología SAx)
    struct TwoPoleLPSAx {
        enum { State = 2, Coeff = 3 };
        static inline_ ::m128 tick(float in, m4f* z, const m4f* k) {
            ::m128 out = z[0];
            z[0] = ::m128(in) * k[0] + z[0] * k[1] + z[1] * k[2];
            z[1] = out;
            return out;
        }
    };

    struct ZeroLP {
        enum { State = 1 };
        static inline_ float tick(float in, float* z) {
            float out = in + z[0];
            z[0] = in;
            return out;
        }
    };

} // ~ namespace sp

// ============================================================================
// 4. INCLUSIONES DE EXTENSIÓN
// ============================================================================

#include "sp/coefficients.h"
#include "sp/more.h"

#endif // ~ SP_SP_INCLUDED
