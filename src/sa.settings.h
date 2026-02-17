#ifndef SA_SETTINGS_INCLUDED
#define SA_SETTINGS_INCLUDED

#include "kali/runtime.h" // [FIX] Necesario para UsesCallback
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

    // [C4 MASTER SYNC] Herencia de callback corregida
    struct Type : kali::UsesCallback {
        int* internalData;
        Type(int* p) : internalData(p) {}
        void operator()(int i, int v, bool notify = true) { 
            if (internalData[i] != v) { internalData[i] = v; if(notify) callback(i); } 
        }
        int operator()(int i) const { return internalData[i]; }
    };
}

namespace config {
    using namespace settings;
    
    // [C4 MASTER SYNC] Constantes requeridas por sa.display.h
    const int pollTime = 48;
    const int infEdge  = -200;
    const int barPad   = 2;
    const int ParameterCount = 32;
    const int SettingsIndex  = 5;

    enum BarType { Bar, Curve, CurveFill };

    struct Defaults {
        const char* operator()(int i, int (&dst)[ParameterCount]) const { return "Default"; }
        const int* data() const { static int v[ParameterCount] = {0}; return v; }
    };
}

// Estructura de estado compartido (Sincronizada)
struct Shared {
    Analyzer* analyzer;
    settings::Type settings;
    int parameter[config::ParameterCount];
    void* display; // Cambiado a void* para evitar circularidad extrema
    void* editor;

    Shared() : analyzer(nullptr), settings(parameter + config::SettingsIndex) {
        std::memset(parameter, 0, sizeof(parameter));
    }
};

} // namespace sa
#endif
