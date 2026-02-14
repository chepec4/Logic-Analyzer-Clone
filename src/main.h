#ifndef PLUGIN_INCLUDED
#define PLUGIN_INCLUDED

// ============================================================================
// 1. CORRECCIÓN DE WINDOWS (Prioridad Máxima)
// ============================================================================
// Incluimos esto PRIMERO para que HINSTANCE y HWND existan
#ifndef WIN32_LEAN_AND_MEAN
 #define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

// ============================================================================
// 2. PARCHES MATEMÁTICOS (SSE & Intrinsics)
// ============================================================================
#include <xmmintrin.h>
#include <emmintrin.h>
#include <pmmintrin.h>
#include <string.h>

// Definimos la macro V que faltaba en coefficients.h
#define V(x) (float)(x)

// Suma horizontal (hsum)
inline float hsum(__m128 x) {
    __m128 t = _mm_add_ps(x, _mm_movehl_ps(x, x));
    __m128 r = _mm_add_ss(t, _mm_shuffle_ps(t, t, 1));
    float f; _mm_store_ss(&f, r); return f;
}

// Shuffle de 1 argumento (ya lo teníamos)
template <int i0, int i1, int i2, int i3>
inline __m128 shuffle(__m128 x) {
    return _mm_shuffle_ps(x, x, _MM_SHUFFLE(i3, i2, i1, i0));
}

// NUEVO: Shuffle de 2 argumentos (El que causaba el error en analyzer.h)
template <int i0, int i1, int i2, int i3>
inline __m128 shuffle(__m128 a, __m128 b) {
    return _mm_shuffle_ps(a, b, _MM_SHUFFLE(i3, i2, i1, i0));
}

// ============================================================================
// 3. PARCHES DE IDENTIDAD Y UTILS
// ============================================================================
#define tf // Anulamos el trace function
#define copy(dest, src, size) memcpy(dest, src, size) // Recuperamos copy

#ifndef NAME
 #define NAME "C4 Analyzer"
#endif
#ifndef COMPANY
 #define COMPANY "C4 Productions"
#endif
#ifndef VERSION
 #define VERSION 1.0
#endif

// ============================================================================
// 4. INCLUDES ORIGINALES
// ============================================================================
#include "preset-handler.h"
#include "sa.legacy.h"
#include "sa.display.h"

// ============================================================================
// 5. PARCHES DE OPERADORES 'SP' (Fuerza Bruta)
// ============================================================================
// Aquí solucionamos el error "no member named data"
namespace sp {
    // Truco: Convertimos el array a puntero __m128 directamente
    template <typename T>
    inline const __m128& as_m128(const T& x) {
        return *reinterpret_cast<const __m128*>(&x);
    }

    // Sobrecarga MULTIPLICACIÓN (Band::T * m128)
    inline __m128 operator*(const array<float, 4, SSE>& a, __m128 b) {
        return _mm_mul_ps(as_m128(a), b);
    }
    
    // Sobrecarga SUMA (Band::T + m128)
    inline __m128 operator+(const array<float, 4, SSE>& a, __m128 b) {
        return _mm_add_ps(as_m128(a), b);
    }
}

// ============================================================================
// 6. CLASE PRINCIPAL
// ============================================================================

struct Plugin :
    sp::AlignedNew <16>,
    PresetHandler <Plugin, 9, sa::config::ParameterCount>
{
    typedef PresetHandler <Plugin, 9, sa::config::ParameterCount> Base;

    void suspend() { tf analyzerUpdate(); }
    void setSampleRate(float rate) { Base::setSampleRate(rate); analyzerUpdate(); }

    void settingsChanged(int index) {
        if (index == sa::settings::bandsPerOctave) analyzerUpdate();
        if (shared.display) shared.display->settingsChanged();
        this->invalidatePreset();
    }

    void analyzerUpdate() {
        using namespace sa::settings;
        const int bpo[] = {3, 4, 6};
        analyzer.update(sampleRate, bpo[shared.settings(bandsPerOctave)]);
    }

    template <typename T> inline_
    void process(const T* const* in, T* const* out, int n) {
        bypass(in, out, n);
        int ch = shared.settings(sa::settings::inputChannel);
        analyzer.process(in, n, ch);
    }

    template <typename T> inline_
    static void bypass(const T* const* in, T* const* out, int n) {
        const T* in0 = in[0]; const T* in1 = in[1];
              T* out0 = out[0];       T* out1 = out[1];
        if ((in0 == out0) && (in1 == out1)) return;
        while (--n >= 0) { *out0++ = *in0++; *out1++ = *in1++; }
    }

    #if PERF
    void processReplacing(float** in, float** out, int n);
    void processDoubleReplacing(double** in, double** out, int n);
    #else
    void processReplacing(float** in, float** out, int n) {process(in, out, n);}
    #if PROCESS_DBL
    void processDoubleReplacing(double** in, double** out, int n) {process(in, out, n);}
    #endif
    #endif

    const int (&getPreset() const)[sa::config::ParameterCount] {return shared.parameter;}

    void setPreset(int (&value)[sa::config::ParameterCount]) {
        using namespace sa::config;
        using sa::config::ParameterCount;
        namespace p = parameters;

        if (!sa::legacy::convertPreset(value)) return;

        bool applyColors = !Settings(prefsKey).get(PrefName()[keepColors], prefs[keepColors].default_);
        int n = applyColors ? Count : ColorsIndex;
        for (int i = 0; i < n; i++) shared.settings(i, value[i + SettingsIndex], false);

        analyzerUpdate();

        if (shared.editor) shared.editor->settingsChanged(applyColors);
        if (shared.display) shared.display->settingsChanged();
        else {
             if (Settings(prefsKey).get(PrefName()[smartDisplay], prefs[smartDisplay].default_)) {
                for (int i = p::w; i <= p::h; i++) shared.parameter[i] = value[i];
             }
        }
        this->invalidatePreset();
    }

    enum {
        UniqueID = 'C4Az',
        Version  = int(VERSION * 1000),
    };

    static const char* name()   {return NAME;}
    static const char* vendor() {return COMPANY;}

    ~Plugin() {
        if (editor) { delete editor; editor = 0; }
        tf
    }

    Plugin(audioMasterCallback master) : Base(master, sa::config::Defaults()) {
        tf
        this->setNumInputs(2);
        this->setNumOutputs(2);
        #if PROCESS_DBL
            this->canDoubleReplacing();
        #endif
        shared.analyzer = &analyzer;
        this->setEditor(new vst::Editor<Plugin, sa::Display>(this));
        shared.settings.callback.to(this, &Plugin::settingsChanged);
        analyzerUpdate();
    }

public:
    sa::Shared shared;
private:
    Analyzer analyzer;
};

#endif // ~ PLUGIN_INCLUDED
