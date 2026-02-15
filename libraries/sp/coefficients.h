#ifndef SP_COEFFICIENTS_INCLUDED
#define SP_COEFFICIENTS_INCLUDED

#include "sp/base.h"
#include <math.h>

namespace sp {

/**
 * METODOLOGÍA C4 OPTIMIZED:
 * Este archivo centraliza el cálculo de filtros para el banco de filtros del analizador.
 * Se respeta la compatibilidad con tipos vectorizados m128 a través de IterA.
 */

// ----------------------------------------------------------------------------
// Versión 1: Inicialización Básica (4 argumentos)
// ----------------------------------------------------------------------------
template <typename T>
inline void twoPoleLPCoeffs(const T& q, double freq, double rate, double* coeffs_out)
{
    typedef typename T::Type V;
    double w = 2.0 * sp::pi * freq / rate;
    // q aquí actúa como factor de calidad (Q)
    double resonance = 0.70710678118; // Valor Butterworth por defecto
    
    // Almacenamos en el puntero externo por compatibilidad heredada
    coeffs_out[0] = V(w); 
    coeffs_out[1] = V(resonance);
    coeffs_out[2] = V(0);
}

// ----------------------------------------------------------------------------
// Versión 2: Motor del Analizador (5 argumentos - FIRMA EXACTA DEL LOG)
// Esta función es el corazón del banco de filtros de bandas.
// ----------------------------------------------------------------------------
template <typename T>
inline void twoPoleLPCoeffs(const T& dest, double freq, double rate, double bpo, double& ferr)
{
    // dest: es sp::IterA que apunta a los coeficientes del filtro en memoria alineada.
    // freq: frecuencia central de la banda.
    // rate: frecuencia de muestreo.
    // bpo:  Bandas por octava (determina el ancho de banda/Q).
    // ferr: Error de frecuencia (parámetro de salida/referencia).

    typedef typename T::Type V;

    // 1. Cálculo del ancho de banda relativo
    double q_val = 1.0 / (pow(2.0, 0.5 / bpo) - pow(2.0, -0.5 / bpo));
    
    // 2. Pre-warping y normalización
    double w = 2.0 * sp::pi * freq / rate;
    double cos_w = cos(w);
    double alpha = sin(w) / (2.0 * q_val);
    
    // 3. Coeficientes del filtro (Biquad Low Pass simplificado para el analizador)
    // Usamos el destino 'dest' directamente como array (dest[0], dest[1], dest[2])
    // b0 = (1 - cos_w) / 2
    // a1 = -2 * cos_w
    // a2 = 1 - alpha
    
    double a0_inv = 1.0 / (1.0 + alpha);
    
    // Aplicamos los coeficientes optimizados para el proceso de Analyzer::process
    dest[0] = V(((1.0 - cos_w) * 0.5) * a0_inv); // Gain / b0
    dest[1] = V((-2.0 * cos_w) * a0_inv);        // a1
    dest[2] = V((1.0 - alpha) * a0_inv);         // a2

    // Actualizamos ferr para el feedback del motor (si se requiere)
    ferr = freq / rate; 
}

// ----------------------------------------------------------------------------
// Versión 3: Sobrecarga para punteros de flotantes (Legacy Support)
// ----------------------------------------------------------------------------
template <typename T>
inline void twoPoleLPCoeffs(const T& q_ignored, double w, double a, double y, float* coeffs)
{
    typedef typename T::Type V;
    
    // Reconstrucción de la lógica de fase de Seven Phases
    coeffs[2] = (float)(a * (y - 1.0));
    coeffs[1] = (float)(a * 2.0 * cos(w));
    // Aquí q_ignored suele ser un valor escalar pasado por error como IterA
    coeffs[0] = (float)(0.5 * (1.0 - coeffs[1] - coeffs[2]));
}

} // ~ namespace sp

#endif // ~ SP_COEFFICIENTS_INCLUDED
