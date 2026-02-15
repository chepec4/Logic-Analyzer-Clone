#ifndef SP_SP_INCLUDED
#define SP_SP_INCLUDED

#include "kali/platform.h"
#include <xmmintrin.h> // SSE
#include <emmintrin.h> // SSE2

// Incluimos base para constantes matemáticas (PI, etc.)
#include "sp/base.h"

// ............................................................................
// SOLUCIÓN DE INFRAESTRUCTURA C4: WRAPPER SSE
// Reemplazamos el sistema original de tipos nativos por una estructura
// para permitir la sobrecarga de operadores en GCC/MinGW.
// ............................................................................

namespace sp {

struct m128
{
    __m128 v;

    // Constructores para conversión transparente
    __forceinline m128() {}
    __forceinline m128(__m128 f) : v(f) {}
    __forceinline m128(float f) : v(_mm_set_ps1(f)) {}
    
    // Operador de casteo automático a __m128 (para intrínsecos)
    __forceinline operator __m128() const { return v; }
};

// Definimos m4f como sinónimo de m128 (Vital para los filtros)
typedef m128 m4f;

// ............................................................................
// OPERADORES ARITMÉTICOS (Sobrecarga C++ Legal)
// ............................................................................

__forceinline m128 operator + (const m128& a, const m128& b) { return _mm_add_ps(a.v, b.v); }
__forceinline m128 operator - (const m128& a, const m128& b) { return _mm_sub_ps(a.v, b.v); }
__forceinline m128 operator * (const m128& a, const m128& b) { return _mm_mul_ps(a.v, b.v); }
__forceinline m128 operator / (const m128& a, const m128& b) { return _mm_div_ps(a.v, b.v); }

// Operadores Lógicos
__forceinline m128 operator & (const m128& a, const m128& b) { return _mm_and_ps(a.v, b.v); }
__forceinline m128 operator | (const m128& a, const m128& b) { return _mm_or_ps(a.v, b.v); }
__forceinline m128 operator ^ (const m128& a, const m128& b) { return _mm_xor_ps(a.v, b.v); }

// Funciones Auxiliares
__forceinline m128 max(const m128& a, const m128& b) { return _mm_max_ps(a.v, b.v); }
__forceinline m128 min(const m128& a, const m128& b) { return _mm_min_ps(a.v, b.v); }

// ............................................................................
// FUNCIONES DE MANIPULACIÓN DE VECTORES
// ............................................................................

template <int a, int b, int c, int d>
__forceinline m128 shuffle(const m128& x, const m128& y)
{
    return _mm_shuffle_ps(x.v, y.v, _MM_SHUFFLE(d, c, b, a));
}

__forceinline m128 hsum(const m128& x)
{
    m128 r = _mm_add_ps(x.v, _mm_movehl_ps(x.v, x.v));
    return _mm_add_ss(r.v, _mm_shuffle_ps(r.v, r.v, 1));
}

// ............................................................................
// FILTROS (Lógica Original Preservada)
// ............................................................................

struct TwoPoleLP
{
    enum
    {
        State = 2,
        Coeff = 3
    };

    // Nota: Usamos m4f (&z)[State] que ahora es válido gracias al typedef
    static inline_ m128 tick(float in, m4f (&z)[State], const m4f (&k)[Coeff])
    {
        // La sintaxis matemática ahora usa nuestros operadores sobrecargados
        m128 out = m128(in) * k[0] 
                 + z[0] * k[1]
                 + z[1] * k[2];
        z[1] = z[0];
        z[0] = out;
        return out;
    }
};

struct TwoPoleLPSAx : TwoPoleLP // useful only for SA due to delayed output
{
    static inline_ m128 tick(float in, m4f (&z)[State], const m4f (&k)[Coeff])
    {
        m128 out = z[0];
        z[0] = m128(in) * k[0]
             + z[0] * k[1]
             + z[1] * k[2];
        z[1] = out;
        return out;
    }
};

// ............................................................................

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

// ............................................................................
// INCLUSIONES DEPENDIENTES
// Incluimos estos archivos AL FINAL porque dependen de que 'm128' y 'm4f' 
// estén ya definidos arriba.
// ............................................................................

#include "sp/coefficients.h"
#include "sp/more.h"

// NOTA: sp/core.h suele contener definiciones de tipos antiguos. 
// Como ya definimos m128/m4f aquí, evitamos incluirlo para no causar conflictos,
// a menos que contenga otras cosas vitales. Si el compilador se queja, descomentar:
// #include "sp/core.h" 

#endif // ~ SP_SP_INCLUDED
