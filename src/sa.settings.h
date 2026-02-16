#ifndef SA_SETTINGS_INCLUDED
#define SA_SETTINGS_INCLUDED

#include "kali/runtime.h"
#include "kali/function.h"
#include "kali/string.h"
#include "kali/settings.h"
#include "kali/geometry.h"
#include "analyzer.h"
#include "version.h"

namespace sa {
namespace settings {

// ============================================================================
// ÍNDICE DE PARÁMETROS (C4 ARCHITECTURE)
// ============================================================================

enum Index {
    inputChannel,
    peakEnable,
    peakDecay,
    avrgEnable,
    avrgTime,
    avrgBarType,
    avrgBarSize,
    holdEnable,
    holdInfinite,
    holdTime,
    holdDecay,
    holdBarType,
    holdBarSize,
    levelCeil,
    levelRange,
    levelGrid,
    freqGridType,
    bandsPerOctave,
    avrgSlope,

    // Colores (Inicio de bloque persistente)
    peakBarColor,
    holdBarColor,
    avrgBarColor,
    gridBorderColor,
    gridLineColor,
    gridLabelColor,
    bkgTopColor,
    bkgBottomColor,

    Count
};

enum {
    ColorsIndex = peakBarColor,
    ColorsCount = Count - ColorsIndex
};

// ============================================================================
// DESCRIPTORES DE PARÁMETROS (Calibración Logic Pro)
// ============================================================================

struct Descriptor {
    Index       index;
    int         min;
    int         max;
    int         step;
    int         default_;
    const char* unit;
    const char* label;
};

const Descriptor descriptor[] = {
    {inputChannel,   0,  3, 1,  2, "Left, Right, Both, Mix", "Channel"},
    {peakEnable,     0,  1, 1,  1, "bool", "On"},
    {peakDecay,      1, 60, 1, 15, "dB/s", "Decay"},
    {avrgEnable,     0,  1, 1,  1, "bool", "On"},
    {avrgTime,     100, 20000, 100, 3000, "s", "Time"},
    {avrgBarType,    0,  2, 1,  1, "Bars, Curve, Curve Fill", "Show As"},
    {avrgBarSize,    1,  4, 1,  3, "px", "Size"},
    {holdEnable,     0,  1, 1,  1, "bool", "On"},
    {holdInfinite,   0,  1, 1,  0, "bool", "Infinite"},
    {holdTime,       0, 20000, 100, 2000, "s", "Time"},
    {holdDecay,      1, 60, 1,  3, "dB/s", "Decay"},
    {holdBarType,    0,  2, 1,  1, "Bars, Curve, Curve Fill", "Show As"},
    {holdBarSize,    1,  4, 1,  2, "px", "Size"},
    {levelCeil,    -80, 20, 1,  0, "dB", "Ceiling"},
    {levelRange,    10, 120, 1, 60, "dB", "Range"},
    {levelGrid,      2, 12, 1,  6, "dB", "Grid"},
    {freqGridType,   0,  1, 1,  0, "Decade, Octave", "Freq. Grid"},
    {bandsPerOctave, 0,  2, 1,  2, "3, 4, 6", "Bands/Octave"},

    // [LOGIC PRO EDICIÓN] Slope de 4.5 dB/octava por defecto (Valor 9 en escala 0.5)
    {avrgSlope,     -6, 12, 1,  9, "dB/oct", "Slope"},

    #define _COL 0x80000000, 0x7FFFFFFF, 1,
    {peakBarColor,    _COL 0xCC2080F0, "argb", "Peak"},
    {holdBarColor,    _COL 0x99FFFFFF, "argb", "Peak Hold"},
    {avrgBarColor,    _COL 0xEBC0C0C0, "argb", "Average"},
    {gridBorderColor, _COL 0x5EFFFFFF, "argb", "Grid Border"},
    {gridLineColor,   _COL 0x21FFFFFF, "argb", "Grid Line"},
    {gridLabelColor,  _COL 0x8FFFFFFF, "argb", "Grid Label"},
    {bkgTopColor,     _COL 0xFF303030, "rgb",  "Background Top"},
    {bkgBottomColor,  _COL 0xFF101010, "rgb",  "Background Bottom"},
    #undef _COL
};

// ============================================================================
// LÓGICA DE DEPENDENCIAS UI
// ============================================================================

struct Depended {
    bool use;
    int  mask;
    int  value;
    static Depended make(bool uu, bool ac=1, int ai=-1, bool bc=1, int bi=-1, bool cc=1, int ci=-1) {
        Depended r = { uu, ((ai>=0)<<ai) + ((bi>=0)<<bi) + ((ci>=0)<<ci),
                           ((ai>=0)*ac<<ai) + ((bi>=0)*bc<<bi) + ((ci>=0)*cc<<ci) };
        return r;
    }
};

const Depended depended[] = {
    #define _ Depended::make
    _ (0), _ (1), _ (0, 1, peakEnable), _ (1), _ (0, 1, avrgEnable),
    _ (0, 1, avrgEnable), _ (0, 1, avrgEnable), _ (1), _ (1, 1, holdEnable),
    _ (0, 1, holdEnable, 0, holdInfinite), _ (0, 1, holdEnable, 0, holdInfinite),
    _ (0, 1, holdEnable), _ (0, 1, holdEnable), _ (0), _ (0), _ (0), _ (0), _ (0),
    _ (0, 1, avrgEnable),
    #undef _
};

// ============================================================================
// TYPE MANAGER (Value storage)
// ============================================================================

struct Type : kali::UsesCallback {
    Type(int* valueArray) : internalData(valueArray) {}

    void defaults() {
        for (int i = 0; i < Count; i++) internalData[i] = descriptor[i].default_;
    }

    void operator () (int i, int v, bool notify = true) {
        v = kali::min(descriptor[i].max, kali::max(descriptor[i].min, v));
        if (internalData[i] != v) {
            internalData[i] = v;
            if (notify) callback(i);
        }
    }

    int operator () (int i) const { return internalData[i]; }
    const char* name(int i) const { return name_[i]; }
    void notify() const { callback(-1); }

private:
    int* internalData;
    kali::EnumNames <Index, Count> name_;
};

// ============================================================================
// WIDGET ADAPTER (UI Translation)
// ============================================================================

struct WidgetAdapter {
    typedef const Descriptor& Desc;

    int range(int i) const { Desc d = descriptor[i]; return (d.max - d.min) / d.step; }
    int value(int i) const { Desc d = descriptor[i]; return (settingsStore(i) - d.min) / d.step; }
    void value(int i, int v) { Desc d = descriptor[i]; settingsStore(i, v * d.step + d.min); }

    void value(int i, const char* v_) {
        char* end; int v;
        if (i == avrgSlope) v = (int)(strtod(v_, &end) * 2.0);
        else v = (!strcmp(descriptor[i].unit, "s")) ? (int)(strtod(v_, &end) * 1000.0 + 0.5) : (int)strtol(v_, &end, 10);
        if (end != v_) settingsStore(i, v);
    }

    kali::string text(int i, int v) const { Desc d = descriptor[i]; return text_(i, v * d.step + d.min); }
    kali::string text(int i) const { return text_(i, settingsStore(i)); }
    
    kali::string label(int i) const {
        Desc d = descriptor[i];
        if (!strcmp(d.unit, "bool") || !strcmp(d.unit, "argb") || !strcmp(d.unit, "rgb")) return d.label;
        if (strstr(d.unit, ", ")) return kali::string("%s:", d.label);
        return kali::string("%s (%s):", d.label, d.unit);
    }

    const char* unit(int i) const { return descriptor[i].unit; }

private:
    kali::string text_(int i, int v) const {
        Desc d = descriptor[i];
        if (i == avrgSlope) return kali::string("%0.1f", 0.5 * (double)v);
        if (!strcmp(d.unit, "s")) return kali::string("%0.1f", 0.001 * (double)v);
        if (!strstr(d.unit, ", ")) return kali::string("%i", v);

        const char* u = d.unit;
        int count = v;
        while (count-- > 0 && strstr(u, ", ")) u = strstr(u, ", ") + 2;
        const char* end = strstr(u, ", ");
        if (end) return kali::string("%.*s", (int)(end - u), u);
        return kali::string("%s", u);
    }

public:
    WidgetAdapter(Type& s) : settingsStore(s) {}

private:
    Type& settingsStore;
};

} // namespace settings

// ============================================================================
// GLOBAL CONFIGURATION
// ============================================================================

namespace config {
    using namespace settings;
    #define KEY_PATH "SOFTWARE\\" COMPANY "\\" NAME
    const char* const prefsKey  = KEY_PATH;
    const char* const colorsKey = KEY_PATH "\\Colours";
    #undef KEY_PATH

    const int presetVersion = 2;
    const int pollTime = 48;    // ms
    const int infEdge  = -200;  // dB
    const int barPad   = 2;
    const kali::Rect gridPad(24, 16, 24, 16);
    const kali::Size displaySize(653, 261);

    enum BarType { Bar, Curve, CurveFill };

    namespace parameters {
        enum Parameters { version, w, h, unused1, unused2, Count };
    }

    enum {
        SettingsIndex  = parameters::Count,
        ParameterCount = parameters::Count + settings::Count
    };

    struct Preset {
        char name[28];
        int value[settings::Count - settings::ColorsCount];
    };

    const Preset preset[] = {
        { "Logic Pro Standard", 2, 1, 15, 1, 3000, 1, 3, 1, 0, 2000, 3, 1, 2, 0, 60, 6, 0, 2, 9 },
        { "Modern Fast",        2, 1, 20, 1,  500, 1, 3, 1, 0,    0, 2, 1, 2, 3, 52, 5, 0, 2, 6 },
        { "Deep Analysis",      3, 0, 12, 1, 4000, 0, 4, 1, 0, 4000, 48, 0, 4, -3, 80, 10, 1, 2, 10 }
    };

    struct Defaults {
        const char* operator () (int index, int (&dst)[ParameterCount]) const {
            std::memcpy(dst, baseValues, sizeof(baseValues));
            int n = sizeof(preset)/sizeof(*preset);
            if ((index > 0) && (index <= n)) {
                std::memcpy(dst + SettingsIndex, preset[index - 1].value, sizeof(preset->value));
                return preset[index - 1].name;
            }
            return index ? ". . ." : "Default";
        }

        Defaults() {
            std::memset(baseValues, 0, sizeof(baseValues));
            baseValues[parameters::version] = presetVersion;
            baseValues[parameters::w] = displaySize.w;
            baseValues[parameters::h] = displaySize.h;
            Type v(baseValues + SettingsIndex);
            v.defaults();
        }
    private:
        int baseValues[ParameterCount];
    };
} // namespace config

// ============================================================================
// SHARED STATE
// ============================================================================

struct Editor;
struct Display;

struct Shared {
    Editor* editor;
    Display* display;
    Analyzer* analyzer;
    settings::Type settings;
    int       parameter[config::ParameterCount];

    Shared() : editor(nullptr), display(nullptr), analyzer(nullptr),
               settings(parameter + config::SettingsIndex) {
        config::Defaults()(0, parameter);
    }
};

} // namespace sa

#endif
