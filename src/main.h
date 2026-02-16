#ifndef MAIN_INCLUDED
#define MAIN_INCLUDED

#include "preset-handler.h"
#include "sa.legacy.h"
#include "sa.display.h"
#include "version.h"

// ============================================================================
// PLUGIN WRAPPER (C4 LOGIC PRO EDITION)
// ============================================================================

struct Plugin :
    sp::AlignedNew <16>,
    PresetHandler <Plugin, 9, sa::config::ParameterCount>
{
    /**
     * [C4 MASTER FIX] Atributos estáticos requeridos por vst::PluginBase
     */
    enum {
        ID = 'C' << 24 | '4' << 16 | 'A' << 8 | 'n',
        ProgramCount = 9,
        ParameterCount = sa::config::ParameterCount
    };

    typedef PresetHandler <Plugin, ProgramCount, ParameterCount> Base;

    sa::Shared shared;
    Analyzer analyzer;

    Plugin(audioMasterCallback master) : Base(master, sa::config::Defaults()), analyzer(44100) {
        shared.analyzer = &analyzer;
        this->setEditor(new vst::Editor<Plugin, sa::Display>(this));
    }

    void analyzerUpdate() {}
    void invalidatePreset() { Base::invalidatePreset(); }
    const int (&getPreset() const)[ParameterCount] { return shared.parameter; }
    void setPreset(int (&v)[ParameterCount]) {}
    
    static const char* name()   { return NAME; }
    static const char* vendor() { return COMPANY; }
};

#endif // ~ MAIN_INCLUDED
