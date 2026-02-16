#ifndef MAIN_INCLUDED
#define MAIN_INCLUDED

#include "preset-handler.h"
#include "sa.legacy.h"
#include "sa.display.h"
#include "version.h"

// ============================================================================
// PLUGIN: IMPLEMENTACIÓN COMPLETA DEL CONTRATO VST
// ============================================================================

struct Plugin :
    sp::AlignedNew <16>,
    PresetHandler <Plugin, 9, sa::config::ParameterCount>
{
    // Atributos estáticos obligatorios para el wrapper vst.h
    enum {
        ID = 'C' << 24 | '4' << 16 | 'A' << 8 | 'n',
        ProgramCount = 9,
        ParameterCount = sa::config::ParameterCount
    };

    typedef PresetHandler <Plugin, ProgramCount, ParameterCount> Base;

    sa::Shared shared;
    Analyzer   analyzer;

    Plugin(audioMasterCallback master) : Base(master, sa::config::Defaults()), analyzer(44100) {
        shared.analyzer = &analyzer;
        // La ventana sa::Display se instancia a través del wrapper VST
        this->setEditor(new vst::Editor<Plugin, sa::Display>(this));
    }

    /**
     * [C4 MASTER FIX] Implementación de función pura.
     * Soluciona: error: invalid new-expression of abstract class type 'Plugin'
     */
    void processReplacing(float** inputs, float** outputs, VstInt32 sampleFrames) override {
        // En un analizador, el audio solo pasa de largo (Passthrough)
        if (inputs[0] != outputs[0]) std::memcpy(outputs[0], inputs[0], sampleFrames * sizeof(float));
        if (inputs[1] != outputs[1]) std::memcpy(outputs[1], inputs[1], sampleFrames * sizeof(float));
        
        // El motor DSP analiza el buffer
        int channelMode = shared.settings(sa::settings::inputChannel);
        analyzer.process(inputs, sampleFrames, channelMode);
    }

    void analyzerUpdate() {
        const int bpo[] = {3, 4, 6};
        analyzer.update(sampleRate, bpo[shared.settings(sa::settings::bandsPerOctave)]);
    }

    void invalidatePreset() { Base::invalidatePreset(); }
    const int (&getPreset() const)[ParameterCount] { return shared.parameter; }
    void setPreset(int (&v)[ParameterCount]) { (void)v; }
    
    static const char* name()   { return NAME; }
    static const char* vendor() { return COMPANY; }
};

#endif
