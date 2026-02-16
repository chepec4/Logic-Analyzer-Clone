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
    // int range() const override { return 0; } // Interface base no tiene range(), native::widget::Interface sí. 
    // Asumimos widget::Interface base de kali/ui/base.h
};

struct Editor : LayerBase {
    typedef Editor This;
    LayerTabsPtr tabs;
    AnyWidget scheme, saveAsDefault;

    // Callback de actualización
    void settingsChanged(bool applyColors) {
        updateSettingsTab();
        if (applyColors) updateColorsTab();
    }

    bool open() override {
        shared.editor = this;
        Font::Scale c(Font::main().scale());
        
        // Crear sistema de pestañas
        tabs = widget::Ctor<LayerTabs>(this, Rect(10, 10, 400, 300));
        if (!tabs) return false;

        initSettingsTab(c);
        initColorsTab(c);

        Size s = tabs->size();
        this->Window::size(s.w + 40, s.h + 80);
        this->Window::text(NAME " Settings" SA_DEBUG_SUFFIX);
        
        // [C4 FIX] Casting explícito a const Window* para resolver ambigüedades
        this->centerAt(reinterpret_cast<const Window*>(shared.display));
        
        return true;
    }

    LayerBase* addLayer(const char* tag) {
        LayerBase* layer = new LayerBase;
        
        // [C4 FIX] Sintaxis correcta para AutoReleasePool 2026
        // app->autorelease.add(layer); // INVALIDO (privado)
        app->autorelease(layer);        // VALIDO
        
        if (!app->loadLayer(tag, this, layer)) {
            return nullptr;
        }
        
        tabs->add(tag, layer);
        return layer;
    }

    // Inicializadores de pestañas (Stubs lógicos)
    void initSettingsTab(Font::Scale& c) {
        LayerBase* layer = addLayer("Settings");
        // ... construcción de UI ...
    }

    void initColorsTab(Font::Scale& c) {
        LayerBase* layer = addLayer("Colors");
        if (!layer) return;
        // ... construcción de UI ...
        // scheme = widget::Ctor<widget::Combo>(this, Rect(0,0,100,20));
    }

    void updateSettingsTab() {}
    void updateColorsTab() {}

    void close() override { 
        shared.editor = nullptr; 
        LayerBase::close(); 
    }

    Editor(Shared& s) : shared(s) {}
    sa::Shared& shared;
};
}
#endif
