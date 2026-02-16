#ifndef SP_COEFFICIENTS_INCLUDED
#define SP_COEFFICIENTS_INCLUDED

#include <cmath>
#include "sp/base.h"

namespace sp {

// Cálculo de coeficientes para TwoPoleLPSAx
template <typename Dest>
inline void twoPoleLPCoeffs(Dest dest, double sampleRate, double freq, double bandwidth, double& ferr)
{
    const double bpo = bandwidth * 0.5;
    const double w0 = 2.0 * sp::pi * freq / sampleRate;
    const double Q = 1.0 / (std::pow(2.0, 0.5 / bpo) - std::pow(2.0, -0.5 / bpo));
    const double alpha = std::sin(w0) / (2.0 * Q);
    const double a0_inv = 1.0 / (1.0 + alpha);

    const float k1 = float((2.0 * std::cos(w0)) * a0_inv);
    const float k2 = float((alpha - 1.0) * a0_inv);
    const float k0 = float(1.0 - k1 - k2);

    dest[0] = k0;
    dest[1] = k1;
    dest[2] = k2;

    ferr = std::abs(freq - (sampleRate * w0 / (2.0 * sp::pi))) / sampleRate;
}

// Sobrecarga simplificada
template <typename Dest>
inline void twoPoleLPCoeffs(Dest dest, double sampleRate, double freq, double bandwidth)
{
    double dummy = 0.0;
    twoPoleLPCoeffs(dest, sampleRate, freq, bandwidth, dummy);
}

} // namespace sp

#endif
