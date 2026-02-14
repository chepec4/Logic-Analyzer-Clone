#ifndef PLUGIN_INCLUDED
#define PLUGIN_INCLUDED

// ============================================================================
// 1. INCLUDES DEL SISTEMA (Limpio, sin tocar WIN32_LEAN_AND_MEAN)
// ============================================================================
#include <string.h>
#include <stdio.h>
#include <math.h>

// ============================================================================
// 2. PARCHES MATEMÁTICOS (SSE) - OBLIGATORIO PARA QUE FUNCIONE EL AUDIO
// ============================================================================
#include <xmmintrin.h>
#include <emmintrin.h>
#include <pmmintrin.h>

// Macro perdida en coefficients.h
#define V(x) (float)(x)

// Suma horizontal (hsum) - Necesaria para curves.h
inline float hsum(__m128 x) {
    __m128 t = _mm_add_ps(x, _mm_movehl_ps(x, x));
    __m128 r = _mm_add_ss(t, _mm_shuffle_ps(t, t, 1));
    float f; _mm_store_ss(&f, r); return f;
}

// Shuffle de 1 argumento
template <int i0, int i1, int i2, int i3>
inline __m128 shuffle(__m128 x) {
    return _mm_shuffle_ps(x, x, _MM_SHUFFLE(i3, i2, i1, i0));
}

// NUEVO: Shuffle de 2 argumentos - Necesario para analyzer.h
template <int i0, int i1, int i2, int i3>
inline __m128 shuffle(__m128 a, __m128 b) {
    return _mm_shuffle_ps(a, b, _MM_SHUFFLE(i3, i2, i1, i0));
}

// ============================================================================
// 3. IDENTIDAD
// ============================================================================
#define tf 
// Recuperamos copy para preset-handler
#define copy(dest, src, size) memcpy(dest, src, size) 

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
// 5. PARCHE DE OPERADORES SP (El arreglo para analyzer.h)
// ============================================================================
namespace sp {
    // Truco para convertir el tipo de array de SP a __m128 nativo
    template <typename T>
    inline const __m128& as_m128(const T& x) {
        return *reinterpret_cast<const __m128*>(&x);
    }
    
    // Operador Multiplicación (*)
    inline __m128 operator*(const array<float, 4, SSE>& a, __m128 b) {
        return _mm_mul_ps(as_m128(a), b);
    }
    // Operador Suma (+)
    inline __m128 operator+(const array<float, 4, SSE>& a, __m128 b) {
        return _mm_add_ps(as_m128(a), b);
    }
    // Función Max
    inline __m128 max(const array<float, 4, SSE>& a, __m128 b) {
        return _mm_max_ps(as_m128(a), b);
    }
}

// ============================================================================
// 6. CLASE PLUGIN
// ============================================================================
struct Plugin : sp::AlignedNew <16>, PresetHandler <Plugin, 9, sa::config::ParameterCount> {
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

    template <typename T> inline_ void process(const T* const* in, T* const* out, int n) {
        bypass(in, out, n);
        int ch = shared.settings(sa::settings::inputChannel);
        analyzer.process(in, n, ch);
    }

    template <typename T> inline_ static void bypass(const T* const* in, T* const* out, int n) {
        const T* in0 = in[0]; const T* in1 = in[1];
              T* out0 = out[0];       T* out1 = out[1];
        if ((in0 == out0) && (in1 == out1)) return;
        while (--n >= 0) { *out0++ = *in0++; *out1++ = *in1++; }
    }

    void processReplacing(float** in, float** out, int n) {process(in, out, n);}
    #if PROCESS_DBL
    void processDoubleReplacing(double** in, double** out, int n) {process(in, out, n);}
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
        else if (Settings(prefsKey).get(PrefName()[smartDisplay], prefs[smartDisplay].default_)) {
            for (int i = p::w; i <= p::h; i++) shared.parameter[i] = value[i];
        }
        this->invalidatePreset();
    }

    enum { UniqueID = 'C4Az', Version  = int(VERSION * 1000) };
    static const char* name()   {return NAME;}
    static const char* vendor() {return COMPANY;}

    ~Plugin() { if (editor) { delete editor; editor = 0; } tf }

    Plugin(audioMasterCallback master) : Base(master, sa::config::Defaults()) {
        tf
        this->setNumInputs(2); this->setNumOutputs(2);
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
#endif
