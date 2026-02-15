#ifndef SP_SP_INCLUDED
#define SP_SP_INCLUDED

#include "kali/platform.h"
#include <xmmintrin.h> // SSE
#include <emmintrin.h> // SSE2
#include "sp/base.h"   // Constantes matemáticas

// ============================================================================
// INFRAESTRUCTURA C4: WRAPPER SSE GLOBAL
// ============================================================================
// Definimos m128 en el espacio global para que analyzer.h lo encuentre sin
// necesitar el prefijo sp::. Usamos una UNION para permitir acceso como array.
// ============================================================================

struct m128
{
    union
    {
        __m128 v;    // Para operaciones SIMD rápidas
        float  f[4]; // Para acceso directo tipo array (k[i])
    };

    // --- COMPATIBILIDAD CON ANALYZER.H ---
    // Definimos los tipos y constantes que analyzer.h busca (T::Type, T::size)
    typedef float Type;
    enum { size = 4 };

    // --- CONSTRUCTORES ---
    __forceinline m128() {}
    __forceinline m128(__m128 i) : v(i) {}
    __forceinline m128(float i) : v(_mm_set_ps1(i)) {} // Broadcast (1.0 -> {1,1,1,1})

    // --- OPERADORES DE CONVERSIÓN ---
    // Permite pasar esta struct a funciones que esperan __m128 nativo
    __forceinline operator __m128() const { return v; }

    // --- ACCESO COMO ARRAY (Fix para error operator[]) ---
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

// Lógicos
__forceinline m128 operator & (const m128& a, const m128& b) { return _mm_and_ps(a.v, b.v); }
__forceinline m128 operator | (const m128& a, const m128& b) { return _mm_or_ps(a.v, b.v); }
__forceinline m128 operator ^ (const m128& a, const m128& b) { return _mm_xor_ps(a.v, b.v); }

// ============================================================================
// FUNCIONES AUXILIARES GLOBALES
// ============================================================================

// Min/Max (Nombres en minúscula para coincidir con llamadas legacy)
__forceinline m128 min(const m128& a, const m128& b) { return _mm_min_ps(a.v, b.v); }
__forceinline m128 max(const m128& a, const m128& b) { return _mm_max_ps(a.v, b.v); }

// Shuffle (Mezcla de componentes)
template <int a, int b, int c, int d>
__forceinline m128 shuffle(const m128& x, const m128& y)
{
    return _mm_shuffle_ps(x.v, y.v, _MM_SHUFFLE(d, c, b, a));
}

// Suma Horizontal (Reduce el vector a un escalar)
__forceinline float hsum(const m128& x)
{
    m128 r = _mm_add_ps(x.v, _mm_movehl_ps(x.v, x.v));
    return _mm_add_ss(r.v, _mm_shuffle_ps(r.v, r.v, 1));
}

// ============================================================================
// ESPACIO DE NOMBRES SP (Filtros y Alias)
// ============================================================================

namespace sp {

// Definimos m4f como un alias de nuestro m128 global
typedef ::m128 m4f; 
typedef ::m128 m128; // Alias interno por si acaso

// ............................................................................
// FILTROS DSP
// ............................................................................

struct TwoPoleLP
{
    enum
    {
        State = 2,
        Coeff = 3
    };

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

struct TwoPoleLPSAx : TwoPoleLP // useful only for SA due to delayed output
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
    enum {State = 1};

    template <typename T> static inline_
    T tick(T in, T (&z)[State])
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
// Importante: coefficients.h y more.h dependen de que m128 ya exista.

#include "sp/coefficients.h"
#include "sp/more.h"

#endif // ~ SP_SP_INCLUDED
