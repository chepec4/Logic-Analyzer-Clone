#ifndef ANALYZER_INCLUDED
#define ANALYZER_INCLUDED

#include "kali/dbgutils.h"
#include "kali/atomic.h"
#include "sp/sp.h"

// ............................................................................

struct Analyzer
{
    template <typename T> noalias_ inline_
    void process(const T* const* in, int samples, int channels)
    {
        enum
        {
            left,
            right,
            both,
            mix
        };

        switch (channels)
        {
            case left  : return process<1, 0>(in + 0, samples);
            case right : return process<1, 0>(in + 1, samples);
            case mix   : return process<1, 1>(in + 0, samples);
            case both  : return process<2, 0>(in, samples);
        }
    }

    template <int Channels, bool Mix, typename T> __declspec(noinline)
    void process(const T* const* in_, int samples)
    {
        const T* in[2] = {in_[0], in_[1]};
        const int n = FrameSize;
        while (samples > 0)
        {
            const int m = n < samples ? n : samples;
            (Channels == 1)
                ? processFrame1ch<Mix>(in, m)
                : processFrame2ch(in, m);
            in[0]   += n;
            in[1]   += n;
            samples -= n;
        }
    }

    template <typename T> inline_
    void processFrame2ch(const T* const* in_, int samples)
    {
        const int n = nBandsPadded();
        for (int i = 0; i < samples; i++)
        {
            float in[2] =
            {
                ZeroLP::tick(float(in_[0][i])
                    + float(sp::adn), zlp[0]),
                ZeroLP::tick(float(in_[1][i])
                    + float(sp::adn), zlp[1])
            };

            sp::m4f prev[2] =
            {
                sp::make_m4f(0,0,0,0),
                sp::make_m4f(0,0,0,0)
            };

            for (int m = 0; m < n; m++)
            {
                sp::m4f x, y;
                Band& b = band[m];

                x = Filter::tick(in[0], b.z[0], b.k);
                y = interband(x, prev[0]);
                prev[0] = x;
                b.p = max(b.p, y * y);
                b.a = b.a + y * y;

                x = Filter::tick(in[1], b.z[1], b.k);
                y = interband(x, prev[1]);
                prev[1] = x;
                b.p = max(b.p, y * y);
                b.a = b.a + y * y;
            }
        }

        savePeaks(peak, band, n, 2 * samples);
    }

    template <bool Mix, typename T> inline_
    void processFrame1ch(const T* const* in_, int samples)
    {
        const int n = nBandsPadded();
        for (int i = 0; i < samples; i++)
        {
            float in = float(in_[0][i]);
            if (Mix)
                in = (in + float(in_[1][i])) * .5f;
            in = ZeroLP::tick(in + float(sp::adn), zlp[0]);

            sp::m4f prev = sp::make_m4f(0,0,0,0);
            for (int m = 0; m < n; m++)
            {
                sp::m4f x, y;
                Band& b = band[m];
                x = Filter::tick(in, b.z[0], b.k);
                y = interband(x, prev);
                prev = x;
                b.p = max(b.p, y * y);
                b.a = b.a + y * y;
            }
        }

        savePeaks(peak, band, n, samples);
    }

    static sp::m4f interband(const sp::m4f& x_in, const sp::m4f& y_in)
    {
        const m128& x = (const m128&)x_in;
        const m128& y = (const m128&)y_in;
        
        m128 res = _mm_sub_ps(x, shuffle<0, 2, 1, 2>
            (shuffle<3, 3, 0, 0>(y, x), x));
            
        return (sp::m4f&)res;
    }

    // ........................................................................

    template <typename Dst, typename Src> // inline_
    void savePeaks(Dst& dst, Src& src, int n, int samples)
    {
        lock.lock();

        sp::m4f zero = sp::make_m4f(0,0,0,0);

        if (clear)
        {
            clear   = false;
            counter = samples;
            for (int i = 0; i < n; i++)
            {
                sp::m4f e = src[i].e;
                dst[i].p = src[i].p * e;
                dst[i].a = src[i].a * e;
                src[i].p = zero;
                src[i].a = zero;
            }
        }
        else
        {
            counter += samples;
            for (int i = 0; i < n; i++)
            {
                sp::m4f e = src[i].e;
                dst[i].p = max(dst[i].p, src[i].p * e);
                dst[i].a = dst[i].a + src[i].a * e;
                src[i].p = zero;
                src[i].a = zero;
            }
        }

        lock.unlock();
    }

    
    template <typename Dst> noalias_ inline_ 
    int readPeaks(Dst& dst) // returns count of processed samples (for avrg)
    {
        lock.lock();

        // FIX: Agregado 'typename' porque Dst es un template parameter
        typename Dst::T* p = &dst.p_;
        typename Dst::T* a = &dst.a_;
        const int n = nBandsPadded();
        for (int i = 0; i < n; i++)
        {
            _mm_store_ps((float*)p, (m128&)peak[i].p);
            _mm_store_ps((float*)a, (m128&)peak[i].a);
            p += 4;
            a += 4;
        }

        clear   = true;
        int ret = counter;
        lock.unlock();
        return ret;
    }

    // ........................................................................

    void update(double fs, int bpo)
    {
        reset();

        const double fmin = 22;
        const double fmax = 29000;

        double f = 15.625;
        double k = exp(sp::ln2 * (1. / bpo));

        while (f < fmin)
            f *= k;
        freqMin = f;

        // ........

        double fanc = 1000;
        double fedg = .94 * .5 * fs;

        int i = int(.5 + log
            (fedg / (fanc * sqrt(k))) / log(k));
        double fli = 1 / (fanc * pow(k, i)); // 1/flast
        double r = (1 - (fedg / sqrt(k)) * fli) * fli * fli;

        double ferr = 1;
        switch (bpo)
        {
            case 3: ferr = .904; break;
            case 4: ferr = .905; break;
            case 6: ferr = .909; break;
        }

        // ........

        int    n  = 0;
        double ff = (f / sqrt(k)) * (1 - r * f * f);
        for (;;)
        {
            sp::twoPoleLPCoeffs
                (sp::IterA <Band::T, float>
                (band[n / Band::N].k, n % Band::N),
                fs, ff, 2 * bpo, ferr);

            double e = emphasis(bpo, ff, fedg);
            band[n / Band::N].e[n % Band::N]
                = float(e * e); // ! we use squared peak and avrg

            ff = f * sqrt(k) * (1 - r * f * f);
            if ((ff > (fedg + 1)) || (ff > fmax))
                break;

            nBands  = ++n;
            freqMax = f;
            f *= k;
        }
    }

private:

    int nBandsPadded() const
    {
        return (1u + nBands + (Band::N - 1)) / Band::N;
    }

    static double emphasis(int bpo, double f, double fedg)
    {
        double base = 1.025; // +0.21dB
        f = f * (1 / fedg);
        if (bpo == 6)
            return base + 2.2 * f * f * f
                - 1.25 * f * f * f * f * f;
        return base + f * f;
    }

    void reset()
    {
        counter = 1;
        clear   = true;
        memset(band, 0, sizeof(band));
        memset(peak, 0, sizeof(peak));
        memset(zlp,  0, sizeof(zlp));
    }

public:

    Analyzer() :
        freqMin (0),
        freqMax (0),
        nBands  (0)
    {reset();}

    typedef sp::ZeroLP         ZeroLP;
    typedef sp::TwoPoleLPSAx   Filter;
    typedef kali::atomic::Lock Lock;

    enum
    {
        MaxBands  = 64,
        FrameSize = 256
    };

    struct Band
    {
        typedef sp::m4f T;
        enum {N = T::size};    // stride

        T k[Filter::Coeff];    // coeff
        T z[2][Filter::State]; // state
        T p;                   // peak
        T a;                   // average
        T e;                   // emphasis
    };

    struct Peak
    {
        typedef Band::T T;
        T p;
        T a;

        struct Out 
        {   
            typedef Band::T::Type T;
            T p_, p[MaxBands - 1],
              a_, a[MaxBands - 1];
        } align_(16);
    };

private:
    Band          band[MaxBands / Band::N];
    Peak          peak[MaxBands / Band::N];
    float         zlp[2][ZeroLP::State];
    int           counter;
    Lock          lock;
    volatile bool clear;

public:
    double        freqMin;
    double        freqMax;
    int           nBands;
};

// ............................................................................

#endif // ~ ANALYZER_INCLUDED
