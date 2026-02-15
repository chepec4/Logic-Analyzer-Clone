#ifndef SP_SP_INCLUDED
#define SP_SP_INCLUDED

#include "kali/platform.h"
#include <xmmintrin.h>
#include <emmintrin.h>
#include <cmath>
#include <cstdlib>
#include <cstring>

// Constantes base (pi, adn, ln2, etc.)
#include "sp/base.h"

// ============================================================================
// ATRIBUTOS DE COMPILADOR
// ============================================================================

#if defined(_MSC_VER)
    #define SP_INLINE __forceinline
#else
    #define SP_INLINE inline __attribute__((always_inline))
#endif

// ============================================================================
// CORE SIMD — ALINEACIÓN DE 16 BYTES (Regla de Oro)
// ============================================================================

struct alignas(16) m128 {
    union {
        __m128 v;
        float  f[4];
    };

    typedef float Type;
    enum { size = 4 };

    SP_INLINE m128() {}
    SP_INLINE m128(__m128 i) : v(i) {}
    SP_INLINE m128(float i)  : v(_mm_set_ps1(i)) {}

    SP_INLINE operator __m128() const { return v; }

    // Acceso directo para depuración y casos específicos
    SP_INLINE float& operator[](int i) { return f[i]; }
    SP_INLINE const float& operator[](int i) const { return f[i]; }
};

typedef m128 m4f;

// ----------------------------------------------------------------------------
// OPERADORES VECTORIALES
// ----------------------------------------------------------------------------

SP_INLINE m128 operator + (m128 a, m128 b) { return _mm_add_ps(a.v, b.v); }
SP_INLINE m128 operator - (m128 a, m128 b) { return _mm_sub_ps(a.v, b.v); }
SP_INLINE m128 operator * (m128 a, m128 b) { return _mm_mul_ps(a.v, b.v); }
SP_INLINE m128 operator / (m128 a, m128 b) { return _mm_div_ps(a.v, b.v); }

// Funciones matemáticas vectoriales
SP_INLINE m128 min(m128 a, m128 b) { return _mm_min_ps(a.v, b.v); }
SP_INLINE m128 max(m128 a, m128 b) { return _mm_max_ps(a.v, b.v); }

template <int a, int b, int c, int d>
SP_INLINE m128 shuffle(m128 x, m128 y) {
    return _mm_shuffle_ps(x.v, y.v, _MM_SHUFFLE(d, c, b, a));
}

SP_INLINE m128 hsum(m128 x) {
    __m128 r = _mm_add_ps(x.v, _mm_movehl_ps(x.v, x.v));
    return _mm_add_ss(r, _mm_shuffle_ps(r, r, 1));
}

// ============================================================================
// NAMESPACE SP — INFRAESTRUCTURA DE AUDIO
// ============================================================================

namespace sp {

    using ::m128;
    using ::m4f;

    // ------------------------------------------------------------------------
    // ITERADOR DE MIEMBROS (Requerido por sa.display.h)
    // ------------------------------------------------------------------------
    template <typename T, typename V, V T::*M>
    struct Iter {
        T* p;
        explicit Iter(T* ptr) : p(ptr) {}
        // [C4 FIX] Devolvemos por valor para evitar error de "discards qualifiers"
        V operator[](int i) const { return p[i].*M; }
    };

    // ------------------------------------------------------------------------
    // ITERADOR DE ARREGLOS (Requerido por coefficients.h)
    // ------------------------------------------------------------------------
    template <typename T, typename V>
    struct IterA {
        V* p;
        // [C4 FIX] Typedef OBLIGATORIO para coefficients.h:45
        typedef V Type;

        IterA(T* base, int offset = 0) {
            p = reinterpret_cast<V*>(reinterpret_cast<char*>(base) + offset);
        }
        SP_INLINE V& operator[](int i) { return p[i]; }
    };

    // ------------------------------------------------------------------------
    // GESTIÓN DE MEMORIA ALINEADA (Requerido por main.h)
    // ------------------------------------------------------------------------
    template <int Align>
    struct AlignedNew {
        void* operator new(size_t size) { return _mm_malloc(size, Align); }
        void operator delete(void* p)   { _mm_free(p); }
    };

    // ------------------------------------------------------------------------
    // FILTROS (Requeridos por analyzer.h)
    // ------------------------------------------------------------------------

    // Zero Latency Pre-filter
    struct ZeroLP {
        enum { State = 1 };
        static SP_INLINE float tick(float in, float* z) {
            float out = in + z[0]; z[0] = in; return out;
        }
    };

    // Banco de filtros principal
    struct TwoPoleLPSAx {
        enum { State = 2, Coeff = 3 };
        static SP_INLINE m128 tick(float in, m4f* z, const m4f* k) {
            m128 input(in);
            m128 out = z[0];
            z[0] = input * k[0] + z[0] * k[1] + z[1] * k[2];
            z[1] = out;
            return out;
        }
    };

} // namespace sp

#include "sp/coefficients.h"
#include "sp/more.h"

#endif
