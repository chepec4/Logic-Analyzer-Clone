#ifndef SA_DISPLAY_INCLUDED
#define SA_DISPLAY_INCLUDED

#include "kali/ui/native.h"
#include "kali/graphics.opengl.h"
#include "analyzer.h"
#include "sa.editor.h"
#include "sa.widgets.h"

namespace sa {

using namespace kali;

// ============================================================================
// ESTRUCTURA DE DATOS DE RENDERIZADO
// ============================================================================

struct DrawData {
    DrawData() : freqMin(0), freqMax(0), nBands(0) {}

    typedef sp::Meter<config::pollTime, config::infEdge> Meter;
    enum { MaxBands = Analyzer::MaxBands };

    Meter  peak[MaxBands];
    double avrg[MaxBands];
    double freqMin;
    double freqMax;
    int    nBands;
};

// ============================================================================
// MOTOR DE VISUALIZACIÓN LOGIC PRO (OpenGL Core)
// ============================================================================

struct Display : DrawData, TrackMousePosition <ui::native::LayerBase> 
{
    // Renderizado de Barras de Energía
    template <bool H, settings::Index Color, typename T>
    void drawBars(int p[][4][2], int h, const T& level) const {
        using namespace config;
        const int n = data->nBands;
        for (int i = 0; i < n; i++) {
            int y = int(gridLevelScale * (settings(levelCeil) - level[i]));
            p[i][1][1] = p[i][2][1] = y;
            p[i][0][1] = p[i][3][1] = y * H + h;
        }
        gl::color(settings(Color));
        glVertexPointer(2, GL_INT, 0, p);
        glDrawArrays(GL_QUADS, 0, n * 4);
    }

    // Renderizado de Curvas de Promedio (Average Curve)
    template <settings::Index Color, settings::Index Width, typename T>
    void drawCurve(float p[][2], int n, const T& level, bool fill) const {
        using namespace config;
        n -= 2;
        const double ceil = settings(levelCeil) * gridLevelScale;
        for (int i = 0; i < n; i++)
            p[i + 1][1] = float(ceil - level[i] * gridLevelScale);

        p[0][1] = p[1][1] * 2 - p[2][1];
        p[n + 1][1] = (p[n - 1][1] < p[n][1]) ? p[n][1] * 2 - p[n - 1][1] : p[n][1];

        unsigned color = settings(Color);
        const float (*fillRect)[2] = nullptr;
        Rect r = gridRect;
        const float pp[4][2] = {
            float(r.x + r.w), float(-1 + r.h), float(r.x), float(-1 + r.h),
            float(r.x), float(-1), float(r.x + r.w), float(-1),
        };

        if (fill) {
            fillRect = pp;
            color = (color & 0xffffff) + (color >> 1u & 0xff000000);
        }
        gl::color(color);
        float width = .667f * (.5f + settings(Width));
        gl::drawCurve_<16>(p, n + 2, fillRect, width);
    }

    // [C4 ARCHITECTURE FIX] drawForeground
    void drawForeground() const {
        using namespace config;
        int x = gridRect.x + barPad + 1;
        int y = gridRect.y + barPad + 1;
        int h = gridRect.h - barPad * 2 - 1;
        int w_area = gridRect.w - barPad * 2 - 1;

        glEnable(GL_SCISSOR_TEST);
        glScissor(x - 1, y, w_area, h);
        glPushMatrix();
        glTranslatef(0 - .5f, y - .5f, 0);
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnable(GL_LINE_SMOOTH);

        const int n = data->nBands;
        int p[MaxBands][4][2];
        
        // [C4 FIX] Explicit kali::max para evitar colisión de namespaces
        int w = kali::max(barWidth, (barWidth + barPad + 1) >> 1);
        int v = barWidth + barPad - w;
        
        for (int i = 0; i < n; i++) {
            p[i][0][0] = p[i][1][0] = x; x += w;
            p[i][2][0] = p[i][3][0] = x; x += v;
        }

        float pp[MaxBands + 2][2];
        float xx = gridRect.x + barPad + (w + 1) * .5f;
        w = barWidth + barPad;
        for (int i = 0; i < n; i++) { pp[i + 1][0] = xx; xx += w; }
        pp[0][0] = pp[1][0] - (w + barPad);
        pp[n + 1][0] = pp[n][0] + (w + barPad);

        const sp::Iter<const Meter, double, &Meter::peak> peakIter(data->peak);
        const sp::Iter<const Meter, double, &Meter::hold> holdIter(data->peak);

        const int holdType = settings(holdBarType);
        if ((holdType != CurveFill) && settings(peakEnable)) drawBars<0, peakBarColor>(p, h, peakIter);
        if (settings(holdEnable)) holdType ? drawCurve<holdBarColor, holdBarSize>(pp, n + 2, holdIter, holdType == CurveFill)
                                          : drawBars<1, holdBarColor>(p, settings(holdBarSize), holdIter);
        if ((holdType == CurveFill) && settings(peakEnable)) drawBars<0, peakBarColor>(p, h, peakIter);
        if (settings(avrgEnable)) settings(avrgBarType) ? drawCurve<avrgBarColor, avrgBarSize>(pp, n + 2, data->avrg, settings(avrgBarType) == CurveFill)
                                                        : drawBars<1, avrgBarColor>(p, settings(avrgBarSize), data->avrg);

        drawPointerInfo();
        glDisable(GL_LINE_SMOOTH);
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisable(GL_SCISSOR_TEST);
        glPopMatrix();
    }

    void drawBackground() {
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT);
        glPushMatrix();
        using namespace config;
        
        // [C4 ARCHITECTURE FIX] Uso de Rect nativo de Window wrapper
        gridRect = Rect(context->size());
        Rect& r = gridRect;

        gl::drawRectDiagonalGradient(Rect(0, 0, r.w, r.h), 0xFF000000 | settings(bkgTopColor), 0xFF000000 | settings(bkgBottomColor));

        int width = r.w - gridPad.x - gridPad.w;
        barWidth = (width - barPad - 1) / data->nBands - barPad;
        width = 1 + barPad + data->nBands * (barWidth + barPad);
        r.x += (r.w - width) / 2; r.w = width;
        r.y += gridPad.y; r.h -= gridPad.y + gridPad.h;

        gridLevelScale = (r.h - barPad * 2 - 2) / double(settings(levelRange));
        glTranslatef(r.x - .5f, r.y - .5f, 0);
        glLineWidth(1);
        gl::color(settings(gridBorderColor));
        gl::drawRectFrame(Rect(0, 0, r.w, r.h));

        // Grilla Logarítmica y Frecuencias
        const double fRatio1 = 1. / data->freqMin;
        const double fRatio2 = 1. / log(fRatio1 * data->freqMax);
        const int x_off = barPad + barWidth / 2;
        const int w_grid = r.w - x_off * 2 - 2;

        const FreqGrid& g = freqGrid[settings(freqGridType)];
        for (int i = 0; i < g.count; i++) {
            int xx = 1 + x_off + int(w_grid * fRatio2 * log(g.freq[i][0] * fRatio1) + .5);
            gl::color(settings(gridLineColor));
            glBegin(GL_LINES); glVertex2i(xx, 1); glVertex2i(xx, r.h); glEnd();
            if (g.freq[i][1] > 0) {
                gl::color(settings(gridLabelColor));
                gl::drawText(freqString(g.freq[i][0]), font, xx + 2, r.h + 11, -3);
            }
        }
        glPopMatrix();
    }

    // ========================================================================
    // CALIBRACIÓN SAGRADA: LOGIC PRO (4.5 dB/octave Tilt)
    // ========================================================================
    void poll() {
        resizer.poll(this->handle);
        Analyzer::Peak p; 
        const double scale = 1. / (double)shared.analyzer->readPeaks(p);

        using namespace config;
        const double inf = pow(10.0, infEdge / 20.0);
        const Meter::Options mo = { settings(peakDecay), settings(holdInfinite), settings(holdTime), settings(holdDecay) };
        const int avgSize = settings(avrgTime) / pollTime;

        // Estándares de la industria (Apple Logic Pro X)
        const double logicSlope = 4.5; 
        const double logicRef = 1000.0;
        const double fMin = shared.analyzer->freqMin;
        const double fMax = shared.analyzer->freqMax;

        for (int i = 0; i < nBands; i++) {
            // Interp logarítmica para coincidir con el banco de filtros
            double freq = fMin * pow(fMax / fMin, (double)i / (nBands - 1));
            
            // Cálculo del TILT Psicoacústico
            double tilt = logicSlope * (log(freq / logicRef) / log(2.0));

            // Procesamiento de Peak con Inyección de Tilt
            double rawPeakDB = sp::g2dB(p.p[i] + (inf * inf));
            peak[i].tick(.5 * (rawPeakDB + tilt), mo);

            // Procesamiento de Average con Inyección de Tilt
            double a = p.a[i] * scale;
            double aDB = sp::g2dB(avrf[i].tick(a, inf * inf, avgSize));
            avrg[i] = .5 * (aDB + tilt);
        }
        draw();
    }

    void resized() {
        if (::IsIconic(handle) || !context) return;
        Size s = size();
        shared.parameter[config::parameters::w] = s.w;
        shared.parameter[config::parameters::h] = s.h;
        context->size(handle);
        settingsChanged();
    }

    void applyParameters() {
        int* v = shared.parameter;
        this->Window::size(v[config::parameters::w], v[config::parameters::h]);
    }

    bool open() {
        shared.display = this;
        applyParameters();
        context = new gl::Context(handle);
        font = new gl::Font("Tahoma", false, -9);
        timer.callback.to(this, &Display::poll);
        timer.start(this, config::pollTime);
        return true;
    }

    // [C4 ARCHITECTURE FIX] Eliminación de destroy()
    void close() {
        context->begin();
        delete font; 
        delete context;
        if (shared.editor) shared.editor->close(); // Uso de close() del Codex
        shared.editor = nullptr; 
        shared.display = nullptr;
    }

    void drawPointerInfo() const; 
    static string freqString(double value);
    bool draw();
    void settingsChanged();
    void reset();
    bool toggleFreeze();
    void openEditor();
    bool mouseDoubleClick();
    bool mouseR(int, int, int);
    bool vstKeyDown(int);
    bool msgHook(LRESULT&, UINT, WPARAM, LPARAM);

    ~Display() {}
    template <typename Plugin> Display(Plugin* plugin) : 
        plugin(plugin), shared(plugin->shared), settings(shared.settings), resizer(plugin),
        context(nullptr), font(nullptr), barWidth(1), freeze(false) { data = this; }

private:
    typedef const settings::Type Settings;
    typedef sp::Averager<(20000 / config::pollTime) + 1> AvrgFilter;
    AudioEffectX* plugin;
    Shared& shared;
    Settings& settings;
    Resizer resizer;
    const DrawData* data;
    AvrgFilter avrf[MaxBands];
    DrawData frozen;
    gl::Context* context;
    gl::Font* font;
    Rect gridRect;
    double gridLevelScale;
    int barWidth;
    bool freeze;
    Timer timer;
};

} // namespace sa

#endif
