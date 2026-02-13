#ifndef SP_INCLUDED
#define SP_INCLUDED

#include "kali/platform.h"

#include <xmmintrin.h>

#include "sp/base.h"
#include "sp/core.h"
#include "sp/coefficients.h"
#include "sp/more.h"

// ============================================================================
// SSE helpers
// ============================================================================

// Operadores SOLO para m128 (SSE nativo)
inline m128 operator + (const m128& a, const m128& b) { return _mm_add_ps(a, b); }
inline m128 operator * (const m128& a, const m128& b) { return _mm_mul_ps(a, b); }
inline m128         max(const m128& a, const m128& b) { return _mm_max_ps(a, b); }

template <int a, int b, int c, int d>
inline m128 shuffle(const m128& x, const m128& y)
{
    return _mm_shuffle_ps(x, y, _MM_SHUFFLE(d, c, b, a));
}

inline m128 hsum(const m128& x)
{
    m128 r = _mm_add_ps(x, _mm_movehl_ps(x, x));
    return _mm_add_ss(r, _mm_shuffle_ps(r, r, 1));
}

// ============================================================================
// FIX CRÍTICO:
// Convertimos m4f -> m128 explícitamente
// ============================================================================

namespace sp {

// m4f es array<float,4,SSE> → lo tratamos como vector SSE
static inline m128 to_m128(const m4f& v)
{
    return _mm_loadu_ps(&v[0]);
}

static inline void from_m128(m4f& v, const m128& x)
{
    _mm_storeu_ps(&v[0], x);
}

// ============================================================================
// DSP
// ============================================================================

struct TwoPoleLP
{
    enum
    {
        State = 2,
        Coeff = 3
    };

    static inline_ m128 tick(float in, m4f (&z)[State], const m4f (&k)[Coeff])
    {
        m128 out =
            _mm_set_ps1(in) * to_m128(k[0]) +
            to_m128(z[0])    * to_m128(k[1]) +
            to_m128(z[1])    * to_m128(k[2]);

        z[1] = z[0];
        from_m128(z[0], out);

        return out;
    }
};

struct TwoPoleLPSAx : TwoPoleLP
{
    static inline_ m128 tick(float in, m4f (&z)[State], const m4f (&k)[Coeff])
    {
        m128 out = to_m128(z[0]);

        m128 next =
            _mm_set_ps1(in) * to_m128(k[0]) +
            to_m128(z[0])   * to_m128(k[1]) +
            to_m128(z[1])   * to_m128(k[2]);

        z[1] = z[0];
        from_m128(z[0], next);

        return out;
    }
};

// ============================================================================

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

#endif // SP_INCLUDED
