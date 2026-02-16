#ifndef SA_SETTINGS_INCLUDED
#define SA_SETTINGS_INCLUDED

#include "kali/runtime.h"
#include "kali/function.h"
#include "kali/string.h"
#include "kali/settings.h"
#include "kali/geometry.h"
#include "analyzer.h"
#include "version.h"
#include <cstring>

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
        int* internalData;
        Type(int* p) : internalData(p) {}
        void operator()(int i, int v, bool notify=true) { 
            if (internalData[i] != v) { internalData[i] = v; if(notify) callback(i); } 
        }
        int operator()(int i) const { return internalData[i]; }
    };
}

namespace config {
    using namespace settings;
    const int ParameterCount = 32; 
    const int SettingsIndex = 5;
    const int pollTime = 48;
    const int infEdge  = -200;

    enum BarType { Bar, Curve, CurveFill };

    struct Defaults {
        const char* operator()(int i, int (&dst)[ParameterCount]) const {
            std::memset(dst, 0, sizeof(dst));
            return "Default";
        }
        const int* data() const { static int v[ParameterCount] = {0}; return v; }
    };
}

// Forward declarations necesarias para Shared
struct Editor;
struct Display;

/**
 * [C4 MASTER SYNC] Definición completa de Shared.
 * Esto elimina el error 'field shared has incomplete type sa::Shared'
 */
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
