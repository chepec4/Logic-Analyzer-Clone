#ifndef SA_SETTINGS_INCLUDED
#define SA_SETTINGS_INCLUDED

#include "kali/runtime.h"
#include "kali/settings.h"
#include "kali/geometry.h"
#include "analyzer.h"
#include "version.h"

namespace sa {

namespace settings {
    enum Index { 
        inputChannel, peakEnable, peakDecay, avrgEnable, avrgTime, 
        avrgBarType, avrgBarSize, holdEnable, holdInfinite, holdTime, 
        holdDecay, holdBarType, holdBarSize, levelCeil, levelRange, 
        levelGrid, freqGridType, bandsPerOctave, avrgSlope, 
        peakBarColor, holdBarColor, avrgBarColor, gridBorderColor, 
        gridLineColor, gridLabelColor, bkgTopColor, bkgBottomColor, Count 
    };

    struct Type : kali::UsesCallback {
        int* data;
        Type(int* p) : data(p) {}
        void operator()(int i, int v, bool notify=true) { data[i] = v; if(notify) callback(i); }
        int operator()(int i) const { return data[i]; }
    };
}

namespace config {
    using namespace settings;
    const int ParameterCount = 32;
    const int SettingsIndex = 5;
    const int barPad = 2; // [FIX] Requerido por sa.display.h
    
    enum BarType { Bar, Curve, CurveFill };

    struct Defaults {
        const char* operator()(int i, int (&dst)[ParameterCount]) const { return "Default"; }
        const int* data() const { static int v[ParameterCount] = {0}; return v; }
    };
}

struct Editor;
struct Display;

// [C4 MASTER FIX] Definición completa de Shared
struct Shared {
    Editor* editor;
    Display* display;
    Analyzer* analyzer;
    settings::Type settings;
    int parameter[config::ParameterCount];

    Shared() : editor(nullptr), display(nullptr), analyzer(nullptr), 
               settings(parameter + config::SettingsIndex) {
        std::memset(parameter, 0, sizeof(parameter));
    }
};

} // namespace sa
#endif
