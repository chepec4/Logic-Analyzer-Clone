#ifndef SP_SP_INCLUDED
#define SP_SP_INCLUDED

#include "kali/platform.h"
#include <xmmintrin.h> // SSE
#include <emmintrin.h> // SSE2
#include "sp/base.h"   // Constantes matemáticas

// ============================================================================
// INFRAESTRUCTURA C4: WRAPPER SSE GLOBAL
// ============================================================================
// Definimos m128 de forma global para compatibilidad con analyzer.h
// ============================================================================

struct m128
{
    union
    {
        __m128 v;    // Registro SIMD
        float  f[4]; // Acceso tipo array
    };

    // Tipos requeridos por las plantillas de analyzer.h
    typedef float Type;
    enum { size = 4 };

    __forceinline m128() {}
    __forceinline m128(__m128 i) : v(i) {}
    __forceinline m128(float i) : v(_mm_set_ps1(i)) {}

    // Conversión a tipo nativo para instrucciones intrínsecas
    __forceinline operator __m128() const { return v; }

    // Operadores de acceso a componentes
    __forceinline float& operator[](int i) { return f[i]; }
    __forceinline const float& operator[](int i) const { return f[i]; }
};

// ============================================================================
// OPERADORES ARITMÉTICOS GLOBALES
// ============================================================================

__forceinline m128 operator + (const m128& a, const m128& b) { return _mm_add_ps(a.v, b.v); }
__forceinline m128 operator - (const m128& a, const m128& b) { return _mm_sub_ps(a.v, b.v); }
__forceinline m128 operator * (const m128& a, const m128& b) { return _mm_mul_ps(a.v, b.v); }
__forceinline m128 operator / (const m128& a, const m128& b) { return _mm_div_ps(a.v, b.v); }

__forceinline m128 operator & (const m128& a, const m128& b) { return _mm_and_ps(a.v, b.v); }
__forceinline m128 operator | (const m128& a, const m128& b) { return _mm_or_ps(a.v, b.v); }
__forceinline m128 operator ^ (const m128& a, const m128& b) { return _mm_xor_ps(a.v, b.v); }

// ============================================================================
// FUNCIONES AUXILIARES
// ============================================================================

__forceinline m128 min(const m128& a, const m128& b) { return _mm_min_ps(a.v, b.v); }
__forceinline m128 max(const m128& a, const m128& b) { return _mm_max_ps(a.v, b.v); }

template <int a, int b, int c, int d>
__forceinline m128 shuffle(const m128& x, const m128& y)
{
    return _mm_shuffle_ps(x.v, y.v, _MM_SHUFFLE(d, c, b, a));
}

// [C4 FIX] Corrección de conversión de tipo en hsum para GCC
__forceinline float hsum(const m128& x)
{
    __m128 r = _mm_add_ps(x.v, _mm_movehl_ps(x.v, x.v));
    __m128 s = _mm_add_ss(r, _mm_shuffle_ps(r, r, 1));
    return _mm_cvtss_f32(s); // Extrae el float bajo del registro correctamente
}

// ============================================================================
// ESPACIO DE NOMBRES SP (FILTROS)
// ============================================================================

namespace sp {

// Alias para los filtros internos
typedef ::m128 m128;
typedef ::m128 m4f;

struct TwoPoleLP
{
    enum { State = 2, Coeff = 3 };

    static inline_ ::m128 tick(float in, m4f (&z)[State], const m4f (&k)[Coeff])
    {
        ::m128 out = ::m128(in) * k[0]
                   + z[0] * k[1]
                   + z[1] * k[2];
        z[1] = z[0];
        z[0] = out;
        return out;
    }
};

struct TwoPoleLPSAx : TwoPoleLP
{
    static inline_ ::m128 tick(float in, m4f (&z)[State], const m4f (&k)[Coeff])
    {
        ::m128 out = z[0];
        z[0] = ::m128(in) * k[0]
             + z[0] * k[1]
             + z[1] * k[2];
        z[1] = out;
        return out;
    }
};

struct ZeroLP
{
    enum { State = 1 };

    template <typename T> 
    static inline_ T tick(T in, T (&z)[State])
    {
        T out = in + z[0];
        z[0]  = in;
        return out;
    }
};

} // ~ namespace sp

// ============================================================================
// INCLUSIONES DEPENDIENTES
// ============================================================================

#include "sp/coefficients.h"
#include "sp/more.h"

#endif // ~ SP_SP_INCLUDED
