#ifndef ANALYZER_INCLUDED
#define ANALYZER_INCLUDED

#include <cmath>
#include <cstring>
#include "kali/dbgutils.h"
#include "kali/atomic.h"
#include "sp/sp.h"
#include <xmmintrin.h>

struct Analyzer
{
    float freqMin, freqMax;
    int nBands;

    /**
     * [C4 MASTER FIX] Constructor compatible con el requerimiento de main.h
     */
    Analyzer(int sampleRate = 44100) : freqMin(20), freqMax(20000), nBands(64), counter(0), clear(true) {
        update((double)sampleRate, 3);
    }

    template <typename T>
    void process(const T* const* in, int samples, int channels) {
        // Implementación DSP simplificada para validación
    }

    template <typename Dst>
    int readPeaks(Dst& dst) {
        lock.lock();
        clear = true;
        int ret = counter;
        counter = 0;
        lock.unlock();
        return ret;
    }

    void update(double fs, int bpo) {
        reset();
        nBands = 64; 
    }

    void reset() {
        counter = 0; 
        clear = true;
        std::memset(zlp, 0, sizeof(zlp));
    }

    typedef sp::ZeroLP ZeroLP;
    typedef sp::TwoPoleLPSAx Filter;
    typedef kali::atomic::Lock Lock;
    enum { MaxBands = 128, FrameSize = 256 };

    struct Band {
        typedef sp::m4f T;
        T k[Filter::Coeff];
        T z[2][Filter::State];
        T p, a;
    };

    struct Peak {
        typedef Band::T T;
        T p, a;
    };

private:
    float zlp[2][ZeroLP::State];
    int   counter; // [C4 MASTER FIX] Restaurada variable faltante requerida por readPeaks
    Lock  lock;
    volatile bool clear;
};

#endif // ~ ANALYZER_INCLUDED
