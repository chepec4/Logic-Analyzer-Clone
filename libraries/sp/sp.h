#ifndef SP_SP_INCLUDED
#define SP_SP_INCLUDED

#include "sp/base.h"
#include <xmmintrin.h> // SSE
#include <emmintrin.h> // SSE2

// ============================================================================
// SOLUCIÓN C4: WRAPPER PARA SSE (GCC COMPATIBLE)
// ============================================================================
// Transformamos el typedef original en una estructura para poder
// sobrecargar los operadores aritméticos legalmente en C++.
// ============================================================================

namespace sp {

struct m128
{
    __m128 v;

    // 1. Constructor por defecto (sin inicializar para rendimiento)
    __forceinline m128() {}

    // 2. Constructor desde el tipo nativo __m128
    __forceinline m128(__m128 f) : v(f) {}

    // 3. Constructor desde float (Broadcast: copia el valor a las 4 posiciones)
    // Esto permite código como: m128 x = 1.0f;
    __forceinline m128(float f) : v(_mm_set_ps1(f)) {}

    // 4. Operador de conversión automática a __m128
    // Esto permite pasar 'm128' a funciones intrínsecas como _mm_add_ps
    __forceinline operator __m128() const { return v; }
};

// ============================================================================
// OPERADORES ARITMÉTICOS (Implementación Inline Optimizada)
// ============================================================================

// Suma (+)
__forceinline m128 operator + (const m128& a, const m128& b) { 
    return _mm_add_ps(a.v, b.v); 
}

// Resta (-)
__forceinline m128 operator - (const m128& a, const m128& b) { 
    return _mm_sub_ps(a.v, b.v); 
}

// Multiplicación (*)
__forceinline m128 operator * (const m128& a, const m128& b) { 
    return _mm_mul_ps(a.v, b.v); 
}

// División (/)
__forceinline m128 operator / (const m128& a, const m128& b) { 
    return _mm_div_ps(a.v, b.v); 
}

// ============================================================================
// OPERADORES LÓGICOS BIT A BIT
// ============================================================================

__forceinline m128 operator & (const m128& a, const m128& b) { 
    return _mm_and_ps(a.v, b.v); 
}

__forceinline m128 operator | (const m128& a, const m128& b) { 
    return _mm_or_ps(a.v, b.v); 
}

__forceinline m128 operator ^ (const m128& a, const m128& b) { 
    return _mm_xor_ps(a.v, b.v); 
}

// ============================================================================
// OPERADORES DE COMPARACIÓN (Retornan máscaras)
// ============================================================================

__forceinline m128 operator == (const m128& a, const m128& b) { 
    return _mm_cmpeq_ps(a.v, b.v); 
}

__forceinline m128 operator < (const m128& a, const m128& b) { 
    return _mm_cmplt_ps(a.v, b.v); 
}

__forceinline m128 operator > (const m128& a, const m128& b) { 
    return _mm_cmpgt_ps(a.v, b.v); 
}

__forceinline m128 operator <= (const m128& a, const m128& b) { 
    return _mm_cmple_ps(a.v, b.v); 
}

__forceinline m128 operator >= (const m128& a, const m128& b) { 
    return _mm_cmpge_ps(a.v, b.v); 
}

// ============================================================================
// FUNCIONES AUXILIARES COMUNES (Usadas por el motor de análisis)
// ============================================================================

// Mínimo
__forceinline m128 min(const m128& a, const m128& b) {
    return _mm_min_ps(a.v, b.v);
}

// Máximo
__forceinline m128 max(const m128& a, const m128& b) {
    return _mm_max_ps(a.v, b.v);
}

// Suma horizontal (reduce el vector sumando sus componentes)
// Necesaria para calcular la energía total de una banda
__forceinline float hsum(const m128& a) {
    m128 t = _mm_add_ps(a.v, _mm_movehl_ps(a.v, a.v));
    t = _mm_add_ss(t.v, _mm_shuffle_ps(t.v, t.v, 1));
    float r;
    _mm_store_ss(&r, t.v);
    return r;
}

// Raíz cuadrada inversa (optimización rápida para normalización)
__forceinline m128 rsqrt(const m128& a) {
    return _mm_rsqrt_ps(a.v);
}

} // ~ namespace sp

#endif // ~ SP_SP_INCLUDED
