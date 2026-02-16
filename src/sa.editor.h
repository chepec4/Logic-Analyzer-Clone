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

struct NullWidget : widget::Interface {
    bool enable() const override {return 0;}
    void enable(bool) override {}
    bool visible() const override {return 0;}
    void visible(bool) override {}
    int  value() const override {return 0;}
    void value(int) override {}
    int  range() const override {return 0;}
    void range(int) override {}
    string text() const override {return string();}
    void text(const char*) override {}
    Window::Handle expose() const override {return 0;};
};

struct Compound : widget::Interface {
    bool   enable() const override {return master->enable();}
    void   enable(bool v) override { master->enable(v); text_->enable(v); label_->enable(v); }
    bool   visible() const override {return master->visible();}
    void   visible(bool v) override { master->visible(v); text_->visible(v); label_->visible(v); }
    int    value() const override {return master->value();}
    void   value(int v) override {master->value(v);}
    int    range() const override {return master->range();}
    void   range(int v) override {master->range(v);}
    string text() const override {return text_->text();}
    void   text(const char* v) override {text_->text(v);}
    Window::Handle expose() const override {return 0;};

    void ctor(AnyWidget m, AnyWidget t, AnyWidget l) {
        master = m; text_ = t; label_ = l;
        master->callback.to(this, &Compound::valueAction);
        text_->extCallback.to(this, &Compound::valueAction);
    }
private:
    void valueAction(int) {callback(value());}
    AnyWidget master, text_, label_;
};

struct Editor : LayerBase {
    typedef Editor This;
    typedef sa::settings::WidgetAdapter Settings;
    
    // [C4 ARCHITECTURE] Punteros concretos para cumplir con el Factory de Kali
    kali::ui::native::widget::Base* tabs;
    Button saveAsDefault;
    Combo  scheme;

    void close() {
        saveCustomColors();
        shared.editor = 0;
        delete this;
    }

    bool open() {
        shared.editor = this;
        installColors();
        Font::Scale c(Font::main().scale());

        // [C4 ARCHITECTURE] Instanciación nativa sin stubs
        tabs = widget::Ctor<kali::ui::native::widget::Base>(this, Rect(6, 7, 400, 300));
        tabs->callback.to(this, &This::tabChanged);
        
        initSettingsTab();
        initColorsTab(c);
        initPrefsTab(c);

        Size s = tabs->size();
        this->Window::size(s.w + 40, s.h + 80);
        this->Window::text(NAME " Settings");
        return true;
    }

    LayerBase* addLayer(const char* tag) {
        LayerBase* layer = new LayerBase;
        // [C4 ARCHITECTURE] Gestión de memoria vía app singleton
        app->autorelease.add(layer);
        if (!app->loadLayer(tag, this, layer)) return 0;
        // La lógica de añadir al control de pestañas nativo
        return layer;
    }

    void initColorsTab(Font::Scale& c) {
        scheme = widget::Ctor<widget::Combo>(this, Rect(0,0,100,20));
        // [C4 ARCHITECTURE] Geometría vía Wrapper Window (Exigido por Codex)
        Rect r(Window(scheme->expose()).position(), Window(scheme->expose()).size());
    }

    // [Stubs lógicos de C4]
    void tabChanged(int v) { saveAsDefault->visible(!v); }
    void initSettingsTab() {}
    void updateSettingsTab() {}
    void initPrefsTab(Font::Scale&) {}
    void updateColorsTab() {}
    void saveCustomColors() {}
    void installColors() {}
    void saveDefaults() {}

    Editor(Shared& s) : shared(s), tabs(0) {}
    sa::Shared&  shared;
    Compound widget[32]; // Max settings count
    ColorWell color[16];
    Compound opacity[16];
    Toggle pref[8];
    NullWidget nullWidget;
};

} // namespace sa
#endif
