#ifndef ANALYZER_INCLUDED
#define ANALYZER_INCLUDED

#include <cmath>
#include <cstring>
#include "kali/dbgutils.h"
#include "kali/atomic.h"
#include "sp/sp.h"

struct Analyzer
{
    // Ruteo de canales (Mantiene lógica original)
    template <typename T> noalias_ inline_
    void process(const T* const* in, int samples, int channels) {
        enum { left, right, both, mix };
        switch (channels) {
            case left:  return process<1, 0>(in + 0, samples);
            case right: return process<1, 0>(in + 1, samples);
            case mix:   return process<1, 1>(in + 0, samples);
            case both:  return process<2, 0>(in,     samples);
        }
    }

    #if defined(_MSC_VER)
    #define NOINLINE __declspec(noinline)
    #else
    #define NOINLINE __attribute__((noinline))
    #endif

    template <int Channels, bool Mix, typename T>
    NOINLINE void process(const T* const* in_, int samples) {
        const T* in[2] = { in_[0], in_[1] };
        const int block = FrameSize;
        while (samples > 0) {
            const int m = block < samples ? block : samples;
            // [C4 FIX] Cambiamos if constexpr por if normal para compatibilidad C++14 
            // si no se actualiza el Makefile, o lo dejamos si actualizamos.
            if (Channels == 1) processFrame1ch<Mix>(in, m);
            else processFrame2ch(in, m);
            in[0] += m; in[1] += m;
            samples -= m;
        }
    }

    template <typename T>
    inline_ void processFrame2ch(const T* const* in_, int samples) {
        const int n = nBandsPadded();
        for (int i = 0; i < samples; ++i) {
            float input[2] = {
                ZeroLP::tick(float(in_[0][i]) + float(sp::adn), zlp[0]),
                ZeroLP::tick(float(in_[1][i]) + float(sp::adn), zlp[1])
            };
            m128 prev[2] = { _mm_setzero_ps(), _mm_setzero_ps() };
            for (int m = 0; m < n; ++m) {
                Band& b = band[m];
                m128 x = Filter::tick(input[0], b.z[0], b.k);
                m128 y = interband(x, prev[0]);
                prev[0] = x;
                b.p = kali::max(b.p, y * y);
                b.a = b.a + y * y;

                x = Filter::tick(input[1], b.z[1], b.k);
                y = interband(x, prev[1]);
                prev[1] = x;
                b.p = kali::max(b.p, y * y);
                b.a = b.a + y * y;
            }
        }
        savePeaks(peak, band, n, 2 * samples);
    }

    template <bool Mix, typename T>
    inline_ void processFrame1ch(const T* const* in_, int samples) {
        const int n = nBandsPadded();
        for (int i = 0; i < samples; ++i) {
            float input = float(in_[0][i]);
            if (Mix) input = (input + float(in_[1][i])) * 0.5f;
            input = ZeroLP::tick(input + float(sp::adn), zlp[0]);
            m128 prev = _mm_setzero_ps();
            for (int m = 0; m < n; ++m) {
                Band& b = band[m];
                m128 x = Filter::tick(input, b.z[0], b.k);
                m128 y = interband(x, prev);
                prev = x;
                b.p = kali::max(b.p, y * y);
                b.a = b.a + y * y;
            }
        }
        savePeaks(peak, band, n, samples);
    }

    static m128 interband(const m128& x, const m128& y) {
        return _mm_sub_ps(x, _mm_shuffle_ps(_mm_shuffle_ps(y, x, _MM_SHUFFLE(3,3,0,0)), x, _MM_SHUFFLE(2,1,2,0)));
    }

    template <typename Dst>
    noalias_ inline_ int readPeaks(Dst& dst) {
        lock.lock();
        // [C4 FIX] Typename obligatorio
        typename Dst::T* p = (typename Dst::T*)&dst.p_;
        typename Dst::T* a = (typename Dst::T*)&dst.a_;
        const int n = nBandsPadded();
        for (int i = 0; i < n; ++i) {
            _mm_store_ps((float*)p, peak[i].p.v);
            _mm_store_ps((float*)a, peak[i].a.v);
            p += 4; a += 4;
        }
        clear = true;
        int ret = counter;
        lock.unlock();
        return ret;
    }

    // ... Resto de la lógica de update y reset (igual a la propuesta anterior) ...
    // Se incluye por integridad en el archivo completo.

    void update(double fs, int bpo) {
        reset();
        const double fmin = 22.0; const double fmax = 29000.0;
        double f = 15.625;
        double k = std::exp(sp::ln2 * (1.0 / bpo));
        while (f < fmin) f *= k;
        freqMin = f;
        double fanc = 1000.0; double fedg = 0.94 * 0.5 * fs;
        int i = int(0.5 + std::log(fedg / (fanc * std::sqrt(k))) / std::log(k));
        double fli = 1.0 / (fanc * std::pow(k, i));
        double r = (1 - (fedg / std::sqrt(k)) * fli) * fli * fli;
        double ferr = 1.0;
        switch (bpo) { case 3: ferr = .904; break; case 4: ferr = .905; break; case 6: ferr = .909; break; }
        int bandIdx = 0;
        double ff = (f / std::sqrt(k)) * (1 - r * f * f);
        for (;;) {
            sp::twoPoleLPCoeffs(sp::IterA<Band::T, float>(band[bandIdx / Band::N].k, bandIdx % Band::N), fs, ff, 2 * bpo, ferr);
            ff = f * std::sqrt(k) * (1 - r * f * f);
            if ((ff > fedg + 1) || (ff > fmax)) break;
            nBands = ++bandIdx;
            freqMax = f; f *= k;
        }
    }

    void reset() {
        counter = 1; clear = true;
        std::memset(band, 0, sizeof(band));
        std::memset(peak, 0, sizeof(peak));
        std::memset(zlp, 0, sizeof(zlp));
    }

    Analyzer() : freqMin(0), freqMax(0), nBands(0) { reset(); }

    typedef sp::ZeroLP ZeroLP;
    typedef sp::TwoPoleLPSAx Filter;
    typedef kali::atomic::Lock Lock;
    enum { MaxBands = 64, FrameSize = 256 };

    struct Band {
        typedef sp::m4f T;
        enum { N = T::size };
        T k[Filter::Coeff];
        T z[2][Filter::State];
        T p, a, e;
    };

    struct Peak {
        typedef Band::T T;
        T p, a;
    };

private:
    template <typename Dst, typename Src>
    void savePeaks(Dst& dst, Src& src, int n, int samples) {
        lock.lock();
        m128 zero = _mm_setzero_ps();
        if (clear) {
            clear = false; counter = samples;
            for (int i = 0; i < n; ++i) {
                dst[i].p = src[i].p; dst[i].a = src[i].a;
                src[i].p = zero; src[i].a = zero;
            }
        } else {
            counter += samples;
            for (int i = 0; i < n; ++i) {
                dst[i].p = kali::max(dst[i].p, src[i].p);
                dst[i].a = dst[i].a + src[i].a;
                src[i].p = zero; src[i].a = zero;
            }
        }
        lock.unlock();
    }

    int nBandsPadded() const { return (1u + nBands + (Band::N - 1)) / Band::N; }

    Band band[MaxBands / Band::N];
    Peak peak[MaxBands / Band::N];
    float zlp[2][ZeroLP::State];
    int counter;
    Lock lock;
    volatile bool clear;

public:
    double freqMin, freqMax;
    int nBands;
};

#endif
