#ifndef SP_SP_INCLUDED
#define SP_SP_INCLUDED

#include "kali/platform.h"
#include <xmmintrin.h> // SSE
#include <emmintrin.h> // SSE2
#include <math.h>

// Aseguramos la existencia de PI para coefficients.h
#ifndef PI
#define PI 3.14159265358979323846
#endif

// ============================================================================
// 1. WRAPPER SSE GLOBAL (Para compatibilidad con analyzer.h)
// ============================================================================

struct m128
{
    union {
        __m128 v;
        float  f[4];
    };

    typedef float Type;
    enum { size = 4 };

    __forceinline m128() {}
    __forceinline m128(__m128 i) : v(i) {}
    __forceinline m128(float i) : v(_mm_set_ps1(i)) {}

    __forceinline operator __m128() const { return v; }

    __forceinline float& operator[](int i) { return f[i]; }
    __forceinline const float& operator[](int i) const { return f[i]; }
};

// Operadores Aritméticos
__forceinline m128 operator + (const m128& a, const m128& b) { return _mm_add_ps(a.v, b.v); }
__forceinline m128 operator - (const m128& a, const m128& b) { return _mm_sub_ps(a.v, b.v); }
__forceinline m128 operator * (const m128& a, const m128& b) { return _mm_mul_ps(a.v, b.v); }
__forceinline m128 operator / (const m128& a, const m128& b) { return _mm_div_ps(a.v, b.v); }

// Operadores Lógicos y de Comparación (Requeridos por el Analizador)
__forceinline m128 operator == (const m128& a, const m128& b) { return _mm_cmpeq_ps(a.v, b.v); }
__forceinline m128 operator <  (const m128& a, const m128& b) { return _mm_cmplt_ps(a.v, b.v); }
__forceinline m128 operator >  (const m128& a, const m128& b) { return _mm_cmpgt_ps(a.v, b.v); }

__forceinline m128 min(const m128& a, const m128& b) { return _mm_min_ps(a.v, b.v); }
__forceinline m128 max(const m128& a, const m128& b) { return _mm_max_ps(a.v, b.v); }

// Fix hsum: Retorna m128 para ser compatible con _mm_store_ss en curves.h
__forceinline m128 hsum(const m128& x)
{
    __m128 r = _mm_add_ps(x.v, _mm_movehl_ps(x.v, x.v));
    return _mm_add_ss(r, _mm_shuffle_ps(r, r, 1));
}

// ============================================================================
// 2. NAMESPACE SP (Infraestructura DSP)
// ============================================================================

namespace sp {

    typedef ::m128 m128;
    typedef ::m128 m4f;

    // Constante para prevenir denormales (Evita picos de CPU)
    static const float adn = 1e-18f;

    // Implementación de IterA para el motor del analizador
    template <typename T, typename V>
    struct IterA {
        T* p;
        IterA(T* p) : p(p) {}
        __forceinline V& operator[](int i) { return ((V*)p)[i]; }
    };

    // Estructuras de Filtros Originales
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

    // Utilidades de conversión
    inline double g2dB(double v) { return (v <= 0) ? -100.0 : 20.0 * log10(v); }

} // ~ namespace sp

// ============================================================================
// 3. INCLUSIONES FINALES
// ============================================================================

#include "sp/coefficients.h"
// Si curves.h o more.h causan problemas, asegúrate de que m128 esté definido antes.
#include "sp/more.h"

#endif // ~ SP_SP_INCLUDED
