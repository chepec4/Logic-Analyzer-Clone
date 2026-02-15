#ifndef SP_COEFFICIENTS_INCLUDED
#define SP_COEFFICIENTS_INCLUDED

#include "sp/core.h"
#include "sp/base.h"

namespace sp {

// ............................................................................

/**
 * Calcula los coeficientes para un filtro de banda de segundo orden (Two-Pole).
 * Adaptado para la estructura m4f (procesamiento de 4 bandas simultáneas).
 * * @param k      Iterador hacia los coeficientes de la banda (k0, k1, k2).
 * @param fs     Frecuencia de muestreo (Sample Rate).
 * @param f      Frecuencia central de la banda.
 * @param bpo    Bandas por octava (resolución).
 * @param ferr   Factor de corrección de error de ancho de banda.
 */
template <typename T>
inline void twoPoleLPCoeffs(T k, double fs, double f, int bpo, double ferr)
{
    // REGLA DE ORO: Todo el cálculo intermedio se hace en double para evitar 
    // acumulación de errores de redondeo antes de bajar a float (m4f).
    
    // 1. Calcular el ancho de banda relativo (Q) basado en las octavas
    double q = ferr * f * (pow(2., .5 / bpo) - pow(2., -.5 / bpo));
    
    // 2. Transformada bilineal para mapear del dominio analógico al digital
    double w = 2 * pi * f / fs;
    double t = tan(w * .5);
    double r = 2 * pi * q / fs;
    double s = 1 / (1 + r * t + t * t);

    // 3. Coeficientes resultantes para la fórmula: out = in*k0 + z0*k1 + z1*k2
    // Estos valores se inyectan en la estructura Band del Analyzer.
    k[0] = float(r * t * s);          // k0 (Ganancia de entrada)
    k[1] = float(2 * s * (1 - t * t));// k1 (Feedback 1)
    k[2] = float(s * (r * t - t * t - 1)); // k2 (Feedback 2)
}

// ............................................................................

/**
 * Tabla de frecuencias centrales sugeridas para paridad con Logic Pro.
 * Logic utiliza centros específicos para que las etiquetas (20, 50, 100...) 
 * queden alineadas visualmente con el pico de los filtros.
 */
struct LogicProFrequencies {
    static double getCenter(int bandIndex, int totalBands) {
        // Implementación de distribución logarítmica pura
        // f = fmin * (fmax/fmin)^(i / (n-1))
        double fMin = 20.0;
        double fMax = 20000.0;
        return fMin * pow(fMax / fMin, (double)bandIndex / (totalBands - 1));
    }
};

// ............................................................................

} // ~ namespace sp

#endif // ~ SP_COEFFICIENTS_INCLUDED
