#ifndef MAIN_INCLUDED
#define MAIN_INCLUDED

#include "preset-handler.h"
#include "sa.legacy.h"
#include "sa.display.h"
#include "version.h"

// [C4 ARCHITECTURE] Diagrama de herencia:
// AudioEffect -> AudioEffectX -> PluginBase -> PresetHandler -> Plugin


struct Plugin :
    sp::AlignedNew <16>,
    PresetHandler <Plugin, 9, sa::config::ParameterCount>
{
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
        this->setEditor(new vst::Editor<Plugin, sa::Display>(this));
    }

    // [C4 MASTER FIX] Firma exacta requerida por el SDK de Steinberg
    void processReplacing(float** inputs, float** outputs, VstInt32 sampleFrames) override {
        // Passthrough
        if (inputs[0] != outputs[0]) std::memcpy(outputs[0], inputs[0], sampleFrames * sizeof(float));
        if (inputs[1] != outputs[1]) std::memcpy(outputs[1], inputs[1], sampleFrames * sizeof(float));
        
        int ch = shared.settings(sa::settings::inputChannel);
        analyzer.process(inputs, sampleFrames, ch);
    }

    void analyzerUpdate() {}
    const int (&getPreset() const)[ParameterCount] { return shared.parameter; }
    void setPreset(int (&v)[ParameterCount]) { (void)v; }
    
    static const char* name()   { return NAME; }
    static const char* vendor() { return COMPANY; }
};

#endif
