#ifndef PLUGIN_INCLUDED
#define PLUGIN_INCLUDED

// 1. INCLUDES DEL SISTEMA (Orden de dependencia estricto)
#ifndef WIN32_LEAN_AND_MEAN
 #define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

// 2. PARCHES SSE (Matemáticas de Audio de alto rendimiento)
#include <xmmintrin.h>
#include <emmintrin.h>
#include <pmmintrin.h>

// Macro de utilidad para casting de precisión
#define V(x) (float)(x)

/**
 * Suma horizontal: Colapsa los 4 canales de un registro SSE en un solo float.
 * Crítico para promediar niveles de bandas.
 */
inline float hsum(__m128 x) {
    __m128 t = _mm_add_ps(x, _mm_movehl_ps(x, x));
    __m128 r = _mm_add_ss(t, _mm_shuffle_ps(t, t, 1));
    float f; _mm_store_ss(&f, r); return f;
}

// Shuffles optimizados (Necesarios para el entrelazado de analyzer.h)
template <int i0, int i1, int i2, int i3>
inline __m128 shuffle(__m128 x) {
    return _mm_shuffle_ps(x, x, _MM_SHUFFLE(i3, i2, i1, i0));
}

template <int i0, int i1, int i2, int i3>
inline __m128 shuffle(__m128 a, __m128 b) {
    return _mm_shuffle_ps(a, b, _MM_SHUFFLE(i3, i2, i1, i0));
}

// 3. IDENTIDAD DEL PROYECTO
#define tf 

#ifndef NAME
 #define NAME "C4 Analyzer"
#endif
#ifndef COMPANY
 #define COMPANY "C4 Productions"
#endif
#ifndef VERSION
 #define VERSION 1.0
#endif

// 4. INCLUDES DEL ECOISTEMA
#include "preset-handler.h"
#include "sa.legacy.h"
#include "sa.display.h"

// 5. OPERADORES SP (Sincronizados con la Union de core.h)
// REGLA DE ORO: Evitamos reinterpret_cast costosos usando acceso directo a la union v128.
namespace sp {
    inline __m128 operator*(const m4f& a, __m128 b) {
        return _mm_mul_ps(a.v128, b);
    }
    inline __m128 operator+(const m4f& a, __m128 b) {
        return _mm_add_ps(a.v128, b);
    }
    inline __m128 max(const m4f& a, __m128 b) {
        return _mm_max_ps(a.v128, b);
    }
}

// 6. CLASE PLUGIN (Punto de convergencia)
/**
 * Estructura Plugin: Implementa AlignedNew para garantizar que el Analyzer
 * no provoque fallos de segmentación en el hardware SSE.
 */
struct Plugin : sp::AlignedNew <16>, PresetHandler <Plugin, 9, sa::config::ParameterCount> {
    typedef PresetHandler <Plugin, 9, sa::config::ParameterCount> Base;

    // Ciclo de vida y actualización
    void suspend() { analyzerUpdate(); }
    void setSampleRate(float rate) { Base::setSampleRate(rate); analyzerUpdate(); }

    void settingsChanged(int index) {
        if (index == sa::settings::bandsPerOctave) analyzerUpdate();
        if (shared.display) shared.display->settingsChanged();
        this->invalidatePreset();
    }

    /**
     * Sincroniza el motor de análisis con el Sample Rate del DAW.
     * Meticulosidad: Mapea las octavas exactas (1/3, 1/4, 1/6).
     */
    void analyzerUpdate() {
        using namespace sa::settings;
        const int bpo[] = {3, 4, 6};
        analyzer.update(sampleRate, bpo[shared.settings(bandsPerOctave)]);
    }

    /**
     * Motor de Proceso: Recibe el audio del DAW y lo inyecta en el Analyzer.
     */
    template <typename T> inline_ void process(const T* const* in, T* const* out, int n) {
        bypass(in, out, n); // Regla de Oro: El audio siempre debe pasar.
        int ch = shared.settings(sa::settings::inputChannel);
        analyzer.process(in, n, ch);
    }

    template <typename T> inline_ static void bypass(const T* const* in, T* const* out, int n) {
        const T* in0 = in[0]; const T* in1 = in[1];
              T* out0 = out[0];       T* out1 = out[1];
        if ((in0 == out0) && (in1 == out1)) return;
        while (--n >= 0) { *out0++ = *in0++; *out1++ = *in1++; }
    }

    void processReplacing(float** in, float** out, int n) { process(in, out, n); }
    
    #if PROCESS_DBL
    void processDoubleReplacing(double** in, double** out, int n) { process(in, out, n); }
    #endif

    const int (&getPreset() const)[sa::config::ParameterCount] { return shared.parameter; }

    /**
     * Gestión de Presets: Asegura que el cambio de colores o bandas
     * actualice la visualización sin colapsar el hilo de audio.
     */
    void setPreset(int (&value)[sa::config::ParameterCount]) {
        using namespace sa::config;
        namespace p = parameters;

        if (!sa::legacy::convertPreset(value)) return;
        
        ::Settings key(prefsKey);
        bool applyColors = !key.get(PrefName()[smartDisplay], prefs[smartDisplay].default_);
        
        int n = applyColors ? Count : ColorsIndex;
        for (int i = 0; i < n; i++) shared.settings(i, value[i + SettingsIndex], false);
        
        analyzerUpdate();
        
        if (shared.editor) shared.editor->settingsChanged(applyColors);
        if (shared.display) shared.display->settingsChanged();
        else if (key.get(PrefName()[smartDisplay], prefs[smartDisplay].default_)) {
            for (int i = p::w; i <= p::h; i++) shared.parameter[i] = value[i];
        }
        this->invalidatePreset();
    }

    enum { UniqueID = 'C4Az', Version = int(VERSION * 1000) };
    static const char* name()   { return NAME; }
    static const char* vendor() { return COMPANY; }

    ~Plugin() { 
        if (editor) { delete editor; editor = 0; } 
    }

    Plugin(audioMasterCallback master) : Base(master, sa::config::Defaults()) {
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
    // El Analyzer reside aquí, beneficiándose de la alineación de Plugin.
    Analyzer analyzer; 
};

#endif // ~ PLUGIN_INCLUDED
