#ifndef SA_EDITOR_INCLUDED
#define SA_EDITOR_INCLUDED

#include "kali/ui/native.h"
#include "sa.settings.h"

namespace sa {
using namespace kali;
using namespace kali::ui::native;

#if defined(_DEBUG) || !defined(NDEBUG)
    #define SA_DEBUG_SUFFIX "-dbg"
#else
    #define SA_DEBUG_SUFFIX ""
#endif

// ============================================================================
// WIDGETS AUXILIARES (Logic Pro Edition)
// ============================================================================

struct NullWidget : widget::Interface {
    bool enable() const override { return false; }
    void enable(bool) override {}
    bool visible() const override { return false; }
    void visible(bool) override {}
    int  value() const override { return 0; }
    void value(int) override {}
    int  range() const override { return 0; }
    void range(int) override {}
    string text() const override { return string(); }
    void text(const char*) override {}
    Window::Handle expose() const override { return nullptr; }
};

struct Compound : widget::Interface {
    bool   enable() const override { return master->enable(); }
    void   enable(bool v) override { master->enable(v); text_->enable(v); label_->enable(v); }
    bool   visible() const override { return master->visible(); }
    void   visible(bool v) override { master->visible(v); text_->visible(v); label_->visible(v); }
    int    value() const override { return master->value(); }
    void   value(int v) override { master->value(v); }
    int    range() const override { return master->range(); }
    void   range(int v) override { master->range(v); }
    string text() const override { return text_->text(); }
    void   text(const char* v) override { text_->text(v); }
    Window::Handle expose() const override { return nullptr; }

    void ctor(AnyWidget m, AnyWidget t, AnyWidget l) {
        master = m; text_ = t; label_ = l;
        master->callback.to(this, &Compound::valueAction);
    }
private:
    void valueAction(int) { callback(value()); }
    AnyWidget master, text_, label_;
};

// ============================================================================
// EDITOR PRINCIPAL
// ============================================================================

struct Editor : LayerBase {
    typedef Editor This;
    
    // [C4 ARCHITECTURE] Uso de Ptr<> sincronizado con native.h
    LayerTabsPtr tabs;
    Button       saveAsDefault;
    Combo        scheme;

    void close() override {
        // saveCustomColors(); // Implementar según lógica de persistencia
        shared.editor = nullptr;
        LayerBase::close(); // Llamada a la base nativa reconstruida
    }

    bool open() override {
        shared.editor = this;
        Font::Scale c(Font::main().scale());

        // [C4 ARCHITECTURE] Instanciación correcta vía Ptr
        tabs = widget::Ctor<LayerTabs>(this, Rect(c.x(6), c.y(7), 400, 300));
        if (!tabs) return false;
        
        tabs->callback.to(this, &This::tabChanged);
        
        initSettingsTab();
        initColorsTab(c);

        Size s = tabs->size();
        this->Window::size(s.w + 40, s.h + 80);
        this->Window::text(NAME " Settings");
        this->centerAt(shared.display); // Uso del centerAt corregido en native.h
        
        return true;
    }

    LayerBase* addLayer(const char* tag) {
        LayerBase* layer = new LayerBase;
        // Registro en el pool de la aplicación (Codex compliance)
        app->autorelease.add(layer);
        
        if (!app->loadLayer(tag, this, layer)) {
            // El pool de autorelease se encargará si falla, pero aquí limpiamos por seguridad
            return nullptr;
        }
        
        // Ahora tabs es LayerTabsPtr, por lo que add() es visible y funcional
        tabs->add(tag, layer);
        return layer;
    }

    void initColorsTab(Font::Scale& c) {
        scheme = widget::Ctor<widget::Combo>(this, Rect(0,0,100,20));
        // Geometría vía Window wrapper como ordena el Codex
        Rect r(Window(scheme->expose()).position(), Window(scheme->expose()).size());
    }

    // --- Lógica de Negocio (Adaptar según sa.settings.h) ---
    void tabChanged(int v) { if(saveAsDefault) saveAsDefault->visible(v == 0); }
    void initSettingsTab() { /* Carga de widgets de configuración */ }

    Editor(Shared& s) : shared(s) {}
    sa::Shared&  shared;
    Compound     widget[32];
    ColorWell    color[16];
    NullWidget   nullWidget;
};

} // namespace sa
#endif
