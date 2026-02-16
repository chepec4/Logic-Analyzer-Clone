#ifndef MAIN_INCLUDED
#define MAIN_INCLUDED

#include "includes.h"
#include "preset-handler.h"
#include "sa.legacy.h"
#include "sa.display.h"
#include "version.h"

// ============================================================================
// DEFINICIÓN PRINCIPAL DEL PLUGIN (C4 Architecture)
// ============================================================================

struct Plugin :
    sp::AlignedNew <16>, // Gestión de memoria alineada para SSE
    PresetHandler <Plugin, 9, sa::config::ParameterCount>
{
    // [C4 FIX] Contratos estáticos requeridos por vst::PluginBase en vst.h
    enum {
        // UniqueID registrado: 'C4An'
        ID = 'C' << 24 | '4' << 16 | 'A' << 8 | 'n',
        Version = int(VERSION * 1000),
        ProgramCount = 9,
        ParameterCount = sa::config::ParameterCount
    };

    typedef PresetHandler <Plugin, ProgramCount, ParameterCount> Base;

    // --- Ciclo de Vida VST ---

    void suspend() { analyzerUpdate(); }

    void setSampleRate(float rate) {
        Base::setSampleRate(rate);
        analyzerUpdate();
    }

    // --- Callbacks de Configuración ---

    void settingsChanged(int index) {
        if (index == sa::settings::bandsPerOctave) analyzerUpdate();
        // Notificar a la UI si está abierta
        if (shared.display) shared.display->settingsChanged();
        this->invalidatePreset();
    }

    void analyzerUpdate() {
        using namespace sa::settings;
        const int bpo[] = {3, 4, 6}; // Mapeo de opciones de bandas
        // Actualizar motor DSP con sample rate y bandas por octava
        analyzer.update(sampleRate, bpo[shared.settings(bandsPerOctave)]);
    }

    // --- Procesamiento de Audio ---

    template <typename T> inline_
    void process(const T* const* in, T* const* out, int n) {
        // 1. Passthrough (Bypass de audio hardwired)
        bypass(in, out, n);
        
        // 2. Análisis (Solo lectura del input)
        int ch = shared.settings(sa::settings::inputChannel);
        analyzer.process(in, n, ch);
    }

    template <typename T> inline_
    static void bypass(const T* const* in, T* const* out, int n) {
        const T* in0 = in[0]; const T* in1 = in[1];
              T* out0 = out[0];       T* out1 = out[1];
        
        // Optimización: Si los buffers son iguales (in-place), no hacer nada
        if ((in0 == out0) && (in1 == out1)) return;
        
        while (--n >= 0) { *out0++ = *in0++; *out1++ = *in1++; }
    }

    // Wrappers VST
#if PERF
    // Stub para profiling si se define PERF
    void processReplacing(float** in, float** out, int n);
    void processDoubleReplacing(double** in, double** out, int n);
#else
    void processReplacing(float** in, float** out, int n)         { process(in, out, n); }
    #if PROCESS_DBL
    void processDoubleReplacing(double** in, double** out, int n) { process(in, out, n); }
    #endif
#endif

    // --- Gestión de Presets ---

    const int (&getPreset() const)[ParameterCount] { return shared.parameter; }

    void setPreset(int (&value)[ParameterCount]) {
        using namespace sa::config;
        
        // Conversión legacy de versiones anteriores
        if (!sa::legacy::convertPreset(value)) return;

        // Leer preferencia global "Keep Colors"
        bool applyColors = !::Settings(prefsKey).get(PrefName()[keepColors], prefs[keepColors].default_);
        
        // Cargar parámetros
        int n = applyColors ? sa::settings::Count : sa::settings::ColorsIndex;
        for (int i = 0; i < n; i++)
            shared.settings(i, value[i + SettingsIndex], false);
        
        analyzerUpdate();

        if (shared.editor) shared.editor->settingsChanged(applyColors);
        
        if (shared.display) shared.display->settingsChanged();
        else {
            // Lógica "Smart Display" si la UI está cerrada
            if (::Settings(prefsKey).get(PrefName()[smartDisplay], prefs[smartDisplay].default_)) {
                namespace p = parameters;
                for (int i = p::w; i <= p::h; i++) shared.parameter[i] = value[i];
            }
        }
        this->invalidatePreset();
    }

    // Metadatos
    static const char* name()   { return NAME; }
    static const char* vendor() { return COMPANY; }

    // Destructor
    ~Plugin() {
        if (editor) { delete editor; editor = nullptr; }
    }

    // Constructor Principal
    Plugin(audioMasterCallback master) 
        : Base(master, sa::config::Defaults()), 
          analyzer(44100) // Inicialización con sample rate default
    {
        this->setNumInputs(2);
        this->setNumOutputs(2);
        #if PROCESS_DBL
            this->canDoubleReplacing();
        #endif

        // Vincular componentes compartidos
        shared.analyzer = &analyzer;
        
        // Inyectar Editor (vst.h maneja la creación de la ventana)
        this->setEditor(new vst::Editor<Plugin, sa::Display>(this));
        
        // Configurar callbacks
        shared.settings.callback.to(this, &Plugin::settingsChanged);

        analyzerUpdate();
    }

public:
    sa::Shared shared; // Estado compartido entre DSP y UI
private:
    Analyzer analyzer; // Motor DSP
};

#endif
