// ============================================================================
// SP LIBRARY UNIT TEST / PLAYGROUND
// No se incluye en el DLL final. Usar para depurar algoritmos DSP aislados.
// ============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h> // _mm_malloc

#include "kali/platform.h"

// Modo Debug para pruebas
#if 0 

#define DBG 1
#include "analyzer.h"

void usage()
{
    int fs[] = {32000, 44100, 48000};
    int nfs  = sizeof(fs)/sizeof(*fs);
    int bo[] = {3, 4, 6};
    int nbo  = sizeof(bo)/sizeof(*bo);

    // [C4 FIX] Constructor actualizado
    Analyzer a(44100); 

    // Test de re-configuración
    for (int i = 0; i < nbo; i++)
        for (int j = 0; j < nfs; j++)
            for (int k = 0; k < 3; k++)
                a.update((float)(fs[j] << k), bo[i]);

    /*
    // Ejemplo de proceso
    const int n = 200;
    // Aligned allocation para SSE
    float* in = (float*)_mm_malloc(n * sizeof(float), 16);
    memset(in, 0, n * sizeof(float));
    in[0] = 1.0f; // Impulso

    const float* ins[] = { in, in };
    a.process(ins, n, 0); // 0 = Left channel
    
    _mm_free(in);
    */
}

#else

// Test de SIMD básico
#define trace printf
#include "sp/sp.h"

using namespace sp;

inline m128 inter(const m128& x, const m128& y)
{
    return _mm_sub_ps(shuffle<0, 2, 0, 2>(x, y), shuffle<1, 3, 1, 3>(x, y));
}

#endif
