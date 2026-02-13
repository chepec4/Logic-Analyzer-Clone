#ifndef SP_INCLUDED
#define SP_INCLUDED

#include "kali/platform.h"

#include "sp/base.h"
#include "sp/core.h"
#include "sp/coefficients.h"
#include "sp/more.h"

// ============================================================================
// DEFINICIÓN SEGURA DE m4f (SIN SSE)
// ============================================================================

namespace sp {

struct m4f
{
    float v[4];

    m4f() {}

    m4f(float a, float b, float c, float d)
    {
        v[0] = a; v[1] = b; v[2] = c; v[3] = d;
    }

    float& operator[](int i)       { return v[i]; }
    float  operator[](int i) const { return v[i]; }
};

// Operadores explícitos
inline m4f operator+(const m4f& a, const m4f& b)
{
    return m4f(
        a[0] + b[0],
        a[1] + b[1],
        a[2] + b[2],
        a[3] + b[3]
    );
}

inline m4f operator*(const m4f& a, const m4f& b)
{
    return m4f(
        a[0] * b[0],
        a[1] * b[1],
        a[2] * b[2],
        a[3] * b[3]
    );
}

inline m4f operator*(float f, const m4f& b)
{
    return m4f(
        f * b[0],
        f * b[1],
        f * b[2],
        f * b[3]
    );
}

// ============================================================================
// DSP
// ============================================================================

struct TwoPoleLP
{
    enum { State = 2, Coeff = 3 };

    static inline_ m4f tick(float in, m4f (&z)[State], const m4f (&k)[Coeff])
    {
        m4f out =
            in   * k[0] +
            z[0] * k[1] +
            z[1] * k[2];

        z[1] = z[0];
        z[0] = out;
        return out;
    }
};

struct TwoPoleLPSAx : TwoPoleLP
{
    static inline_ m4f tick(float in, m4f (&z)[State], const m4f (&k)[Coeff])
    {
        m4f out = z[0];
        z[0] =
            in   * k[0] +
            z[0] * k[1] +
            z[1] * k[2];
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

} // namespace sp

#endif
