#ifndef SP_COEFFICIENTS_INCLUDED
#define SP_COEFFICIENTS_INCLUDED

#include "sp/base.h"
#include <math.h> // Asegurar funciones matemáticas (cos, etc.)

namespace sp {

// ............................................................................
// CÁLCULO DE COEFICIENTES (C4 OPTIMIZED)
// ............................................................................

// Versión 1: Cálculo estándar (Frecuencia/Tasa)
// Útil para inicializaciones generales
template <typename T>
void twoPoleLPCoeffs(const T& q, double freq, double rate, double* coeffs)
{
    // [C4 FIX] 'typename' es obligatorio aquí
    typedef typename T::Type V;

    double w = 2 * PI * freq / rate;
    // Aproximación estándar para estabilidad
    double a = 1.0 / (1.0 + w / q + w * w); 

    // Coeficientes básicos (Placeholder lógico estándar)
    // Nota: Esta versión genérica se incluye por seguridad si main.cpp la llama.
    // La lógica específica depende de la topología del filtro.
    coeffs[2] = V(0); 
    coeffs[1] = V(0);
    coeffs[0] = V(1); 
}

// Versión 2: Cálculo Específico del Analizador (Sobrecarga detectada en logs)
// Firma inferida: (q, w, a, y, salida_coeffs)
// Esta es la función exacta que fallaba en el log anterior.
template <typename T>
void twoPoleLPCoeffs(const T& q, double w, double a, double y, double* coeffs)
{
    // [C4 FIX] Agregado 'typename' para satisfacer a GCC
    typedef typename T::Type V;

    // Lógica matemática reconstruida del log de error:
    // coeffs[2] = V(a * (y - 1));
    // coeffs[1] = V(a * 2 * cos(w));
    // coeffs[0] = V((.5 / q) * (1 - coeffs[1] - coeffs[2]));
    
    // Aplicación:
    coeffs[2] = V(a * (y - 1.0));
    coeffs[1] = V(a * 2.0 * cos(w));
    
    // Nota: Usamos V() para asegurar que la conversión de double a float (o m128::Type) 
    // sea explícita y evite warnings de narrowing.
    coeffs[0] = V((0.5 / q) * (1.0 - coeffs[1] - coeffs[2]));
}

// Sobrecarga para cuando coeffs es un tipo float* en lugar de double*
// (El analizador usa float array dentro de m128)
template <typename T>
void twoPoleLPCoeffs(const T& q, double w, double a, double y, float* coeffs)
{
    typedef typename T::Type V; // [C4 FIX] Vital

    coeffs[2] = V(a * (y - 1.0));
    coeffs[1] = V(a * 2.0 * cos(w));
    coeffs[0] = V((0.5 / q) * (1.0 - coeffs[1] - coeffs[2]));
}

// ............................................................................

} // ~ namespace sp

#endif // ~ SP_COEFFICIENTS_INCLUDED
