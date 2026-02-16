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
    inputChannel,
    peakEnable, peakDecay,
    avrgEnable, avrgTime, avrgBarType, avrgBarSize,
    holdEnable, holdInfinite, holdTime, holdDecay, holdBarType, holdBarSize,
    levelCeil, levelRange, levelGrid,
    freqGridType, bandsPerOctave,
    avrgSlope,
    peakBarColor, holdBarColor, avrgBarColor,
    gridBorderColor, gridLineColor, gridLabelColor,
    bkgTopColor, bkgBottomColor,
    Count
};

enum { ColorsIndex = peakBarColor, ColorsCount = Count - ColorsIndex };

struct Descriptor {
    Index index; int min, max, step, default_;
    const char* unit; const char* label;
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

struct Type : kali::UsesCallback {
    Type(int* valueArray) : internalData(valueArray) {}
    void defaults() { for (int i = 0; i < Count; i++) internalData[i] = descriptor[i].default_; }
    void operator () (int i, int v, bool notify = true) {
        v = kali::min(descriptor[i].max, kali::max(descriptor[i].min, v));
        if (internalData[i] != v) { internalData[i] = v; if (notify) callback(i); }
    }
    int operator () (int i) const { return internalData[i]; }
private:
    int* internalData;
    kali::EnumNames <Index, Count> name_;
};

} // namespace settings

namespace config {
    using namespace settings;
    #define KEY_PATH "SOFTWARE\\" COMPANY "\\" NAME
    const char* const prefsKey  = KEY_PATH;
    #undef KEY_PATH

    const int presetVersion = 2;
    const int pollTime = 48;
    const int infEdge  = -200;
    const int barPad   = 2;
    const kali::Rect gridPad(24, 16, 24, 16);
    const kali::Size displaySize(653, 261);

    namespace parameters { enum Parameters { version, w, h, unused1, unused2, Count }; }
    enum { SettingsIndex = parameters::Count, ParameterCount = parameters::Count + settings::Count };

    struct Defaults {
        const char* operator () (int index, int (&dst)[ParameterCount]) const {
            std::memcpy(dst, baseValues, sizeof(baseValues));
            return index ? ". . ." : "Default";
        }
        const int* data() const { return baseValues; }
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

    struct FreqGrid { int count; const double (*freq)[2]; };
    const double freqGridDec[][2] = { {25, 1}, {40, 0}, {50, 1}, {100, 1}, {1000, 1}, {10000, 1}, {20000, 1} };
    const double freqGridLin[][2] = { {31.25, 1}, {62.5, 1}, {125, 1}, {1000, 1}, {16000, 1} };
    const FreqGrid freqGrid[] = {
        { sizeof(freqGridDec)/sizeof(*freqGridDec), freqGridDec },
        { sizeof(freqGridLin)/sizeof(*freqGridLin), freqGridLin }
    };

    enum PrefIndex { keepColors, smartDisplay, PrefCount };
    typedef kali::EnumNames <PrefIndex, PrefCount> PrefName;
    const struct { PrefIndex index; int default_; const char* label; } prefs[PrefCount] = { 
        { keepColors, 1, "Keep colors" }, { smartDisplay, 0, "Smart size" } 
    };
} // namespace config

struct Editor;
struct Display;

struct Shared {
    Editor* editor;
    Display* display;
    Analyzer* analyzer;
    settings::Type settings;
    int parameter[config::ParameterCount];

    Shared() : editor(nullptr), display(nullptr), analyzer(nullptr),
               settings(parameter + config::SettingsIndex) {
        config::Defaults()(0, parameter);
    }
};

} // namespace sa

#endif // SA_SETTINGS_INCLUDED
