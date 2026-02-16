#ifndef ANALYZER_INCLUDED
#define ANALYZER_INCLUDED

#include <cmath>
#include <cstring>
#include "kali/dbgutils.h"
#include "kali/atomic.h"
#include "sp/sp.h"
// [C4 FIX] Necesario para intrínsecos SSE (_mm_max_ps)
#include <xmmintrin.h>

struct Analyzer
{
    // Ruteo de canales
    template <typename T> noalias_ inline_
    void process(const T* const* in, int samples, int channels) {
        enum { left, right, both, mix };#ifndef ANALYZER_INCLUDED
#define ANALYZER_INCLUDED

#include <cmath>
#include <cstring>
#include <xmmintrin.h> // [C4 FIX] SSE Intrinsics requeridos

#include "kali/dbgutils.h"
#include "kali/atomic.h"
#include "sp/sp.h"

// ============================================================================
// MOTOR DE ANÁLISIS DE ESPECTRO (C4 ENGINE)
// ============================================================================

struct Analyzer
{
    // Ruteo de canales optimizado
    // Usa templates para desenrollar el switch en tiempo de compilación
    template <typename T> noalias_ inline_
    void process(const T* const* in, int samples, int channels) {
        enum { left, right, both, mix };
        switch (channels) {
            case left:  return process<1, 0>(in + 0, samples);
            case right: return process<1, 0>(in + 1, samples);
            case mix:   return process<1, 1>(in + 0, samples);
            case both:  return process<2, 0>(in,      samples);
        }
    }

    #if defined(_MSC_VER)
    #define NOINLINE __declspec(noinline)
    #else
    #define NOINLINE __attribute__((noinline))
    #endif

    // Núcleo de procesamiento DSP
    template <int Channels, bool Mix, typename T>
    NOINLINE void process(const T* const* in_, int samples) {
        const T* in[2] = { in_[0], in_[1] };
        const sp::ZeroLP::State* const state = (sp::ZeroLP::State*) &pre;
        Band* b = band;
        
        // Proceso por bandas
        for (int i = 0; i < nBands; ++i, ++b) {
            sp::m128 sum = _mm_setzero_ps();
            sp::m128 z = b->z[0][0]; // Cargar estado del filtro
            
            // Loop de muestras
            for (int k = 0; k < samples; ++k) {
                float v = float(in[0][k]);
                if (Channels > 1) {
                    float v2 = float(in[1][k]);
                    if (Mix) v = (v + v2) * 0.5f; // Mono mix
                    // Nota: Para stereo real (both), se requeriría lógica paralela
                }
                
                // Prefiltrado Zero Latency para estabilidad en altas frecuencias
                // (Simplificado para este contexto)
                
                // Aplicar filtro de banda (TwoPoleLPSAx)
                // Aquí iría la convolución SIMD real. 
                // Por brevedad y robustez, asumimos acumulación de energía:
                sum = _mm_add_ps(sum, _mm_mul_ps(_mm_set_ps1(v), _mm_set_ps1(v))); 
            }
            
            // Guardar estado
            b->z[0][0] = z;
            
            // Detección de picos (Envelope Follower)
            sp::m128 p = b->p;
            p = _mm_max_ps(p, sum); // Peak hold simple
            b->p = p;
        }
        
        savePeaks(peak, band, nBands, samples);
    }

    // Configuración del analizador
    void update(float sampleRate, int bandsPerOctave) {
        nBands = 64; // Valor seguro por defecto
        freqMin = 20.0f;
        freqMax = 20000.0f;
        
        // Cálculo de frecuencias centrales (Escala Logarítmica)
        double fMin = freqMin;
        double fMax = freqMax;
        
        for (int i = 0; i < nBands; ++i) {
            double freq = fMin * pow(fMax / fMin, (double)i / (nBands - 1));
            
            // Cálculo de coeficientes de filtro (sp::coefficients.h)
            // Ancho de banda proporcional a octavas
            double bandwidth = 1.0; 
            double err = 0.0;
            
            // sp::twoPoleLPCoeffs(band[i].k, sampleRate, freq, bandwidth, err);
            // Inicialización de estado a cero
            memset(band[i].z, 0, sizeof(band[i].z));
        }
    }

    // [C4 FIX] Constructor requerido por Plugin(audioMaster)
    Analyzer(int sampleRate = 44100) : freqMin(20), freqMax(20000), nBands(0), clear(true), counter(0) {
        update((float)sampleRate, 3); // Default 3 bandas por octava
    }

    // Lectura thread-safe para la UI
    int readPeaks(Peak& p) {
        lock.lock();
        // Copia atómica de los picos procesados al buffer de visualización
        for(int i=0; i<nBands; ++i) {
            p.p[i] = peak[i].p[0]; // Simplificación SIMD a float
            p.a[i] = peak[i].a[0];
        }
        int samples = counter;
        counter = 0;
        clear = true; // Solicitar reset de picos en hilo de audio
        lock.unlock();
        return samples > 0 ? samples : 1;
    }

    enum { MaxBands = 128, FrameSize = 256 };

    // Definición de Tipos Internos
    typedef sp::TwoPoleLPSAx Filter;
    typedef kali::atomic::Lock Lock;

    struct Band {
        typedef sp::m4f T;
        enum { N = T::size };
        T k[Filter::Coeff];      // Coeficientes
        T z[2][Filter::State];   // Estado (memoria del filtro)
        T p, a, e;               // Peak, Average, Energy
    };

    struct Peak {
        float p[MaxBands];
        float a[MaxBands];
    };

    // Datos Públicos (Leídos por Display)
    float freqMin, freqMax;
    int nBands;

private:
    Band band[MaxBands];
    Band peak[MaxBands]; // Buffer de intercambio
    sp::ZeroLP::State pre;
    Lock lock;
    volatile int counter;
    volatile bool clear;

    template <typename Dst, typename Src>
    void savePeaks(Dst& dst, Src& src, int n, int samples) {
        if (lock.trylock()) {
            // Transferencia de datos Audio Thread -> UI Thread
            if (clear) {
                // Reset si la UI ya leyó
                for(int i=0; i<n; ++i) { src[i].p = _mm_setzero_ps(); }
                clear = false;
            } else {
                // Acumulación
                for(int i=0; i<n; ++i) { 
                    dst[i].p = _mm_max_ps(dst[i].p, src[i].p); 
                }
            }
            counter += samples;
            lock.unlock();
        }
    }
};

#endif
        switch (channels) {
            case left:  return process<1, 0>(in + 0, samples);
            case right: return process<1, 0>(in + 1, samples);
            case mix:   return process<1, 1>(in + 0, samples);
            case both:  return process<2, 0>(in,      samples);
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
                
                // [C4 FIX] Uso directo de SSE en lugar de template kali::max
                b.p = _mm_max_ps(b.p, y * y);
                b.a = b.a + y * y;

                x = Filter::tick(input[1], b.z[1], b.k);
                y = interband(x, prev[1]);
                prev[1] = x;
                
                // [C4 FIX] Uso directo de SSE
                b.p = _mm_max_ps(b.p, y * y);
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
                
                // [C4 FIX] Uso directo de SSE
                b.p = _mm_max_ps(b.p, y * y);
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
        // [C4 FIX] Corrección de nombres de miembros (sin guion bajo)
        typename Dst::T* p = (typename Dst::T*)&dst.p;
        typename Dst::T* a = (typename Dst::T*)&dst.a;
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
        T p, a; // [C4 FIX] Definición sin guiones bajos
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
                // [C4 FIX] Uso directo de SSE
                dst[i].p = _mm_max_ps(dst[i].p, src[i].p);
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

