#ifndef SP_CURVES_INCLUDED
#define SP_CURVES_INCLUDED

#include "sp/sp.h"

namespace sp {

template <int Segments, int Tension = 50>
struct CardinalSpline
{
    typedef float T;
    static int count(int n) { return (n - 1) * Segments + 1; }

    void operator() (T (*dst_)[2], const T (* restrict_ src)[2], int n) const
    {
        T (* restrict_ dst)[Segments][2] = (T (*)[Segments][2]) dst_;
        m128 z[2] = {_mm_set_ps(src[1][0], src[0][0], src[0][0], 0),
                     _mm_set_ps(src[1][1], src[0][1], src[0][1], 0)};
        src += 2;
        while (--n > 2) iter(*dst++, z, *src++);
        iter(*dst++, z, *src);
        iter(*dst++, z, *src);
        (**dst)[0] = (*src)[0];
        (**dst)[1] = (*src)[1];
    }

    CardinalSpline() : k(ctor()) {}

private:
    typedef const sp::m4f* const restrict_ K;
    K k;

    inline void iter(T (* restrict_ dst)[2], m128 (&z)[2], const T (&src_)[2]) const
    {
        m128 src = _mm_loadu_ps(src_);
        z[0] = shuffle<1, 2, 3, 0>(z[0], shuffle<0, 0, 3, 3>(src, z[0]));
        z[1] = shuffle<1, 2, 3, 0>(z[1], shuffle<1, 1, 3, 3>(src, z[1]));

        for (int i = 0; i < Segments; i++)
        {
            _mm_store_ss(dst[i] + 0, hsum(z[0] * k[i]));
            _mm_store_ss(dst[i] + 1, hsum(z[1] * k[i]));
        }
    }

    struct Coeff
    {
        sp::m4f k[Segments];
        operator K() const {return k;}
        Coeff()
        {
            for (int i = 0; i < Segments; i++)
            {
                const T t  = T(.01 * Tension), s_ = T(i * (1. / Segments)),
                      s[4] = {0, s_, s_ * s_, s_ * s_ * s_};
                k[i][0] = -s[3] * t + 2 * (s[2] * t) - s[1] * t;
                k[i][1] =  s[3] * (2 - t) - s[2] * (3 - t) + 1;
                k[i][2] = -s[3] * (2 - t) + s[2] * (3 - 2 * t) + s[1] * t;
                k[i][3] =  s[3] * t - s[2] * t;
            }
        }
    };

    static K ctor()
    {
        static const Coeff k;
        return k;
    }
};

} // ~ namespace sp

#endif
