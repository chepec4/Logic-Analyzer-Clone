#ifndef SA_EDITOR_INCLUDED
#define SA_EDITOR_INCLUDED

#include "kali/ui/native.h"
#include "sa.settings.h"

namespace sa {
using namespace kali;
using namespace kali::ui::native;

struct Editor : LayerBase {
    typedef Editor This;
    LayerTabsPtr tabs;
    AnyWidget scheme, saveAsDefault;

    void settingsChanged(bool applyColors) {
        updateSettingsTab();
        if (applyColors) updateColorsTab();
    }

    bool open() override {
        shared.editor = this;
        // [FIX] Acceso calificado a Font
        kali::ui::native::Font::Scale c(kali::ui::native::Font::main().scale());
        
        // [FIX] Ctor sincronizado con el namespace widget
        tabs = widget::Ctor<widget::LayerTabs>(this, Rect(10, 10, 400, 300));
        if (!tabs) return false;

        initSettingsTab(c);
        initColorsTab(c);

        Size s = tabs->size();
        this->Window::size(s.w + 40, s.h + 80);
        this->Window::text(NAME " Settings");
        this->centerAt(reinterpret_cast<const Window*>(shared.display));
        
        return true;
    }

    LayerBase* addLayer(const char* tag) {
        LayerBase* layer = new LayerBase;
        app->autorelease(layer);
        if (!app->loadLayer(tag, this, layer)) return nullptr;
        tabs->add(tag, layer);
        return layer;
    }

    void initSettingsTab(const kali::ui::native::Font::Scale& c) { }
    void initColorsTab(const kali::ui::native::Font::Scale& c) { }
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
