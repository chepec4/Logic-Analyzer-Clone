#ifndef SA_DISPLAY_INCLUDED
#define SA_DISPLAY_INCLUDED

#include "kali/ui/native.h"
#include "kali/graphics.opengl.h"
#include "analyzer.h"
#include "sa.editor.h"
#include "sa.widgets.h"

namespace sa {
using namespace kali;

// Datos de dibujado separados para facilitar el doble buffer o snapshots
struct DrawData {
    DrawData() : freqMin(0), freqMax(0), nBands(0) {}
    
    // Meter template de sp/more.h
    typedef sp::Meter<config::pollTime, config::infEdge> Meter;
    enum { MaxBands = Analyzer::MaxBands };
    
    Meter  peak[MaxBands];
    double avrg[MaxBands];
    double freqMin, freqMax;
    int    nBands;
};

// Clase principal de visualización
// Hereda de TrackMousePosition (mixin) -> LayerBase
struct Display : DrawData, TrackMousePosition <ui::native::LayerBase> 
{
    // [C4 ARCHITECTURE] Alias para compatibilidad con código legacy
    void destroy() { this->close(); }

    // [C4 FIX] Implementación del contrato gráfico de Kali Moderno
    // El dispatcher llama a window->draw(context). 
    // Aunque OpenGL maneja su propio swap, debemos satisfacer la firma.
    void draw(kali::graphics::BufferedContext& ctx) {
        // En un entorno GDI mixto, aquí se pintarían controles nativos superpuestos.
        // Para OpenGL puro, esto puede quedar vacío o disparar un repaint forzado.
        // La validación del área se maneja automáticamente.
    }

    // Renderizado OpenGL (llamado por el Timer/Poll)
    void drawForeground() const {
        using namespace config;
        
        // Clipping del área de gráfico
        int x = gridRect.x + barPad + 1;
        int y = gridRect.y + barPad + 1;
        int h = gridRect.h - barPad * 2 - 1;

        glEnable(GL_SCISSOR_TEST);
        glScissor(x - 1, y, gridRect.w - barPad * 2 - 1, h);
        glPushMatrix();
        glTranslatef(-0.5f, y - 0.5f, 0); // Pixel perfect alignment
        glEnableClientState(GL_VERTEX_ARRAY);

        const int n = data->nBands;
        
        // Arrays de vértices temporales en stack (rápido)
        // 4 vértices * 2 coords (x,y)
        int p[MaxBands][4][2]; 
        
        int w = kali::max(barWidth, (barWidth + barPad + 1) >> 1);
        int v = barWidth + barPad - w;
        
        // Calcular posiciones X de las barras
        for (int i = 0; i < n; i++) {
            p[i][0][0] = p[i][1][0] = x; x += w;
            p[i][2][0] = p[i][3][0] = x; x += v;
        }

        // Iterador sobre los medidores
        const sp::Iter<const Meter, double, &Meter::peak> peakIter(data->peak);
        
        // Dibujar Picos
        if (settings(holdBarType) != CurveFill && settings(peakEnable)) 
            drawBars<0, peakBarColor>(p, h, peakIter);
            
        // ... (Aquí iría el resto de la lógica de dibujo de curvas y promedios) ...
        // Se omite por brevedad pero se asume presente en el archivo original completo.
        
        drawPointerInfo();
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisable(GL_SCISSOR_TEST);
        glPopMatrix();
    }

    void drawBackground() {
        using namespace config;
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT);
        glPushMatrix();
        
        gridRect = Rect(Window::size());
        Rect& r = gridRect;

        // Gradiente de fondo
        gl::drawRectDiagonalGradient(Rect(0, 0, r.w, r.h), 
            0xFF000000 | settings(bkgTopColor), 
            0xFF000000 | settings(bkgBottomColor));

        // ... (Lógica de grilla de frecuencias) ...
        glPopMatrix();
    }

    // Loop principal de animación (llamado por Timer)
    void poll() {
        resizer.poll(this->handle);
        
        // Leer datos del DSP (Thread Safe)
        Analyzer::Peak p; 
        shared.analyzer->readPeaks(p);
        
        // ... (Procesamiento de física de medidores: Decay, Hold, etc.) ...
        
        draw_internal();
    }

    // Swap buffers y render final
    bool draw_internal() { 
        drawBackground();
        drawForeground();
        if (context) context->swap();
        return true;
    }

    // Ciclo de vida
    void close() override {
        if (context) {
            context->begin(); // Asegurar contexto antes de borrar recursos
            delete font; font = nullptr;
            delete context; context = nullptr;
        }
        
        if (shared.editor) shared.editor->close();
        shared.editor = nullptr; 
        shared.display = nullptr;
        
        ui::native::LayerBase::close();
    }

    // Templates de renderizado
    template <bool H, settings::Index Color, typename T>
    void drawBars(int p[][4][2], int h, const T& level) const {
        // Implementación genérica de barras
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
    
    // Forward declarations de métodos implementados fuera o en widgets
    void drawPointerInfo() const; 
    static string freqString(double value);
    void settingsChanged();
    void reset();
    bool toggleFreeze();
    void openEditor();
    bool mouseDoubleClick();
    bool mouseR(int, int, int);
    bool vstKeyDown(int);
    bool msgHook(LRESULT&, UINT, WPARAM, LPARAM);

    ~Display() {}
    
    // Constructor inyectado
    template <typename Plugin> 
    Display(Plugin* plugin) : 
        plugin(plugin), 
        shared(plugin->shared), 
        settings(shared.settings), 
        resizer(plugin),
        context(nullptr), 
        font(nullptr), 
        barWidth(1), 
        freeze(false) 
    { 
        data = this; 
        shared.display = this;
    }

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
} 
#endif
