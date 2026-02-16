#ifndef SA_DISPLAY_INCLUDED
#define SA_DISPLAY_INCLUDED

#include "kali/ui/native.h"
#include "kali/graphics.opengl.h"
#include "analyzer.h"
#include "sa.editor.h"
#include "sa.widgets.h"

namespace sa {
using namespace kali;

struct DrawData {
    DrawData() : freqMin(0), freqMax(0), nBands(0) {}
    typedef sp::Meter<config::pollTime, config::infEdge> Meter;
    enum { MaxBands = Analyzer::MaxBands };
    Meter  peak[MaxBands];
    double avrg[MaxBands];
    double freqMin, freqMax;
    int    nBands;
};

struct Display : DrawData, TrackMousePosition <ui::native::LayerBase> 
{
    // Implementación del dibujado nativo
    void draw(kali::graphics::BufferedContext& ctx) {
        ctx.begin();
        drawBackground();
        drawForeground();
        ctx.swap();
    }
    
    void drawForeground() const {
        // Soluciona: 'barPad' was not declared in this scope
        using namespace sa::config; 
        int x = gridRect.x + barPad + 1;
        int y = gridRect.y + barPad + 1;
        int h = gridRect.h - barPad * 2 - 1;

        glEnable(GL_SCISSOR_TEST);
        glScissor(x - 1, y, gridRect.w - barPad * 2 - 1, h);
        glPushMatrix();
    }

    void drawBackground() {
        using namespace sa::config;
        Rect& r = gridRect;
        gl::drawRectDiagonalGradient(Rect(0, 0, r.w, r.h), settings(bkgTopColor), settings(bkgBottomColor));
    }

    template <settings::Index Color, typename T>
    void drawBars(int (*p)[4][2], int h, const T& level) const {
        (void)p; (void)h; (void)level;
        gl::color(settings(Color));
    }

    void settingsChanged() {}
    
    void close() override { 
        if (context) {
            delete font; font = nullptr; // Soluciona: 'font' was not declared
            delete context; context = nullptr;
        }
        ui::native::LayerBase::close(); 
    }

    template <typename Plugin> Display(Plugin* plugin) : 
        plugin(plugin), shared(plugin->shared), settings(shared.settings), resizer(plugin),
        context(nullptr), font(nullptr) 
    { data = this; }

private:
    AudioEffectX* plugin;
    Shared& shared;
    const settings::Type& settings;
    Resizer resizer;
    const DrawData* data;
    
    gl::Context* context;
    gl::Font* font; // Declaración del miembro faltante
    
    Rect gridRect;
};

} // namespace sa
#endif
