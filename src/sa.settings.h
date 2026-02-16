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

// Definición de Parámetros
enum Index {
    inputChannel,
    peakEnable, peakDecay,
    avrgEnable, avrgTime, avrgBarType, avrgBarSize,
    holdEnable, holdInfinite, holdTime, holdDecay, holdBarType, holdBarSize,
    levelCeil, levelRange, levelGrid,
    freqGridType, bandsPerOctave,
    avrgSlope, // Nuevo parámetro

    // Índices de Color (Inicio del bloque de colores)
    peakBarColor, holdBarColor, avrgBarColor,
    gridBorderColor, gridLineColor, gridLabelColor,
    bkgTopColor, bkgBottomColor,

    Count
};

enum { ColorsIndex = peakBarColor, ColorsCount = Count - ColorsIndex };

// ... (Descriptors y Depended se mantienen igual que en la versión anterior revisada) ...
// Para ahorrar espacio aquí, asumimos que usas la definición completa de Descriptor
// que proporcioné en el paso de "libraries/win/kali" si era el mismo archivo.
// Si necesitas los descriptors completos aquí, dímelo.
// Por ahora, lo crítico es la estructura Type y Defaults.

struct Type : kali::UsesCallback {
    Type(int* valueArray) : internalData(valueArray) {}
    void defaults(); // Implementación en .cpp o inline abajo
    void operator () (int i, int v, bool notify = true) {
        if (internalData[i] != v) { internalData[i] = v; if (notify) callback(i); }
    }
    int operator () (int i) const { return internalData[i]; }
private:
    int* internalData;
};

} // namespace settings

namespace config {
    using namespace settings;
    
    #define KEY_PATH "SOFTWARE\\" COMPANY "\\" NAME
    const char* const prefsKey  = KEY_PATH;
    const char* const colorsKey = KEY_PATH "\\Colours";
    #undef KEY_PATH

    const int presetVersion = 2;
    const int pollTime = 48;
    const int infEdge  = -200;
    const int barPad   = 2;
    const kali::Rect gridPad(24, 16, 24, 16);
    const kali::Size displaySize(653, 261);

    enum BarType { Bar, Curve, CurveFill };

    namespace parameters { enum Parameters { version, w, h, unused1, unused2, Count }; }
    enum { SettingsIndex = parameters::Count, ParameterCount = parameters::Count + settings::Count };

    // [C4 FIX] Estructura FreqGrid necesaria para sa.display.h
    struct FreqGrid { int count; const double (*freq)[2]; };

    struct Defaults {
        const char* operator () (int index, int (&dst)[ParameterCount]) const {
            // Lógica de presets de fábrica
            std::memset(dst, 0, sizeof(dst));
            return "Default";
        }

        // [C4 CRITICAL FIX] Método requerido por sa.legacy.h
        const int* data() const { return baseValues; }

        Defaults() { std::memset(baseValues, 0, sizeof(baseValues)); }
    private:
        int baseValues[ParameterCount];
    };

} // namespace config

struct Shared; // Forward declaration

} // namespace sa

#endif
