#ifndef SP_INCLUDED
#define SP_INCLUDED

// Incluimos las definiciones base del sistema
#include "kali/platform.h"
#include "sp/base.h"
#include "sp/core.h"
#include "sp/coefficients.h"
#include "sp/more.h"

// ============================================================================
// SOLUCION DE CONFLICTOS Y OPERADORES
// ============================================================================

namespace sp {

// TRUCO DE INGENIERIA:
// Como 'm4f' (definido en core.h) no tiene constructor que acepte (a,b,c,d),
// creamos esta funcion ayudante para fabricarlos manualmente.
static inline m4f make_m4f(float a, float b, float c, float d)
{
    m4f temp;
    temp[0] = a;
    temp[1] = b;
    temp[2] = c;
    temp[3] = d;
    return temp;
}

// ----------------------------------------------------------------------------
// Operadores Matematicos Manuales (Sin SSE para evitar errores de compilador)
// ----------------------------------------------------------------------------

// Suma (+)
inline m4f operator+(const m4f& a, const m4f& b)
{
    return make_m4f(
        a[0] + b[0],
        a[1] + b[1],
        a[2] + b[2],
        a[3] + b[3]
    );
}

// Multiplicacion (*) vector con vector
inline m4f operator*(const m4f& a, const m4f& b)
{
    return make_m4f(
        a[0] * b[0],
        a[1] * b[1],
        a[2] * b[2],
        a[3] * b[3]
    );
}

// Multiplicacion (*) numero con vector
inline m4f operator*(float s, const m4f& a)
{
    return make_m4f(
        s * a[0],
        s * a[1],
        s * a[2],
        s * a[3]
    );
}

// Multiplicacion (*) vector con numero
inline m4f operator*(const m4f& a, float s)
{
    return make_m4f(
        a[0] * s,
        a[1] * s,
        a[2] * s,
        a[3] * s
    );
}

// Maximo entre dos vectores
inline m4f max(const m4f& a, const m4f& b)
{
    return make_m4f(
        (a[0] > b[0]) ? a[0] : b[0],
        (a[1] > b[1]) ? a[1] : b[1],
        (a[2] > b[2]) ? a[2] : b[2],
        (a[3] > b[3]) ? a[3] : b[3]
    );
}

// ----------------------------------------------------------------------------
// DSP - Estructuras de Filtros (Adaptadas para usar make_m4f)
// ----------------------------------------------------------------------------

struct TwoPoleLP
{
    enum { State = 2, Coeff = 3 };

    static inline m4f tick(float in, m4f (&z)[State], const m4f (&k)[Coeff])
    {
        // Formula: out = in*k0 + z0*k1 + z1*k2
        // Desglosamos paso a paso para evitar confusion al compilador
        m4f term1 = in * k[0];
        m4f term2 = z[0] * k[1];
        m4f term3 = z[1] * k[2];
        
        m4f out = term1 + term2 + term3;

        z[1] = z[0];
        z[0] = out;

        return out;
    }
};

struct TwoPoleLPSAx : TwoPoleLP
{
    static inline m4f tick(float in, m4f (&z)[State], const m4f (&k)[Coeff])
    {
        m4f out = z[0];

        m4f term1 = in * k[0];
        m4f term2 = z[0] * k[1];
        m4f term3 = z[1] * k[2];

        m4f next = term1 + term2 + term3;

        z[1] = z[0];
        z[0] = next;

        return out;
    }
};

struct ZeroLP
{
    enum { State = 1 };
    template <typename T>
    static inline T tick(T in, T (&z)[State])
    {
        T out = in + z[0];
        z[0]  = in;
        return out;
    }
};

} // namespace sp

#endif // SP_INCLUDED
