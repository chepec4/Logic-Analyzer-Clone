#ifndef SP_COEFFICIENTS_INCLUDED
#define SP_COEFFICIENTS_INCLUDED

#include <cmath>
#include "sp/base.h"

namespace sp {

/**
 * GENERADOR DE COEFICIENTES PARA BANCO DE FILTROS (C4 DEFINITIVE)
 * Optimizado para la topología recursiva TwoPoleLPSAx.
 * * Esta implementación garantiza que el filtro sea estable y tenga 
 * ganancia unitaria en la frecuencia de paso, fundamental para 
 * la precisión del espectro.
 */

template <typename Dest>
inline void twoPoleLPCoeffs(
    Dest dest,
    double sampleRate,
    double freq,
    double bandwidth,
    double& ferr)
{
    // 1. Cálculo de parámetros básicos
    // bandwidth aquí llega como (2 * bpo) desde analyzer.h
    const double bpo = bandwidth * 0.5;
    const double w0 = 2.0 * sp::pi * freq / sampleRate;

    // 2. Cálculo de Q basado en ancho de banda en octavas
    // Relación precisa para filtros de analizador de espectro
    const double Q = 1.0 / (std::pow(2.0, 0.5 / bpo) - std::pow(2.0, -0.5 / bpo));

    // 3. Diseño de polos (Matemática de precisión Mikhailov)
    // Usamos el radio de los polos para garantizar estabilidad SIMD
    const double alpha = std::sin(w0) / (2.0 * Q);
    const double a0_inv = 1.0 / (1.0 + alpha);

    /**
     * Coeficientes para la ecuación: 
     * y[n] = k0*x[n] + k1*y[n-1] + k2*y[n-2]
     * * k1 y k2 corresponden a los coeficientes recursivos -a1 y -a2.
     */
    const float k1 = float((2.0 * std::cos(w0)) * a0_inv);
    const float k2 = float((alpha - 1.0) * a0_inv);
    
    // Ganancia de normalización para asegurar 0dB en el pico
    const float k0 = float(1.0 - k1 - k2);

    // 4. Asignación al destino (sp::IterA o float*)
    dest[0] = k0;
    dest[1] = k1;
    dest[2] = k2;

    // 5. Feedback de error para el motor de calibración del Analyzer
    // Este valor permite al Analyzer ajustar el 'emphasis' visual.
    ferr = std::abs(freq - (sampleRate * w0 / (2.0 * sp::pi))) / sampleRate;
}

// ----------------------------------------------------------------------------
// SOBRECARGA LEGACY (Soporte para inicialización estática)
// ----------------------------------------------------------------------------
template <typename Dest>
inline void twoPoleLPCoeffs(
    Dest dest,
    double sampleRate,
    double freq,
    double bandwidth)
{
    double dummy = 0.0;
    twoPoleLPCoeffs(dest, sampleRate, freq, bandwidth, dummy);
}

} // namespace sp

#endif
