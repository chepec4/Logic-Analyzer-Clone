#ifndef SA_EDITOR_INCLUDED
#define SA_EDITOR_INCLUDED

#include "kali/ui/native.h"
#include "sa.settings.h"
#include "sa.self-install.h"

namespace sa {

using namespace kali;
using namespace kali::ui::native;

// Determinación de modo Debug para evitar el error 'defined'
#if defined(_DEBUG) || !defined(NDEBUG)
    #define SA_DEBUG_SUFFIX "-dbg"
#else
    #define SA_DEBUG_SUFFIX ""
#endif

// ............................................................................

struct NullWidget : widget::Interface
{
    bool enable() const    {return 0;}
    void enable(bool)      {}
    bool visible() const   {return 0;}
    void visible(bool)     {}
    int  value() const     {return 0;}
    void value(int)        {}
    int  range() const     {return 0;}
    void range(int)        {}
    string text() const    {return string();}
    void text(const char*) {}
    Window::Handle expose() const {return 0;};
    void destroy() {} // [C4 FIX] Requerido por la jerarquía
};

struct Compound : widget::Interface
{
    bool   enable() const       {return master->enable();}
    bool   visible() const      {return master->visible();}
    int    value() const        {return master->value();}
    void   value(int v)         {master->value(v);}
    int    range() const        {return master->range();}
    void   range(int v)         {master->range(v);}
    string text() const         {return text_->text();}
    void   text(const char* v)  {text_->text(v);}
    void   label(const char* v) {label_->text(v);}
    void   destroy() { master->destroy(); text_->destroy(); label_->destroy(); }

    void enable(bool v)
    {
        master->enable(v);
        text_->enable(v);
        label_->enable(v);
    }

    void visible(bool v)
    {
        master->visible(v);
        text_->visible(v);
        label_->visible(v);
    }

    void ctor(AnyWidget m, AnyWidget t, AnyWidget l)
    {
        master = m;
        text_  = t;
        label_ = l;
        master->callback.to(this, &Compound::valueAction);
        text_->extCallback.to(this, &Compound::valueAction);
        text_->callback.to(this, &Compound::textAction);
    }

private:
    void valueAction(int) {callback(value());}
    void textAction(int)  {textCallback(0);}
    Window::Handle expose() const {return 0;};
    Compound(const Compound&);
    Compound& operator = (const Compound&);

public:
    Callback textCallback;
    Compound() {}

private:
    AnyWidget master;
    AnyWidget text_;
    AnyWidget label_;
};

// ............................................................................

struct Editor : LayerBase
{
    typedef Editor This;
    typedef sa::settings::WidgetAdapter Settings;
    typedef const widget::ResourceCtor  Ctor;

    void close()
    {
        saveCustomColors();
        shared.editor = 0;
        this->destroy(); // [C4 FIX] Limpieza segura vía Kali
        delete this;
    }

    bool open()
    {
        shared.editor = this;
        installColors();

        Font::Scale c(Font::main().scale());
        int x = c.x(6);
        int y = c.y(7);

        tabs = widget::Ctor<LayerTabs>(this, Rect(x, y, 400, 300)); // Tamaño base
        tabs->callback.to(this, &This::tabChanged);
        
        initSettingsTab();
        initColorsTab(c);
        initPrefsTab(c);

        Size s = tabs->size();

        widget::Ctor<TextRight>(this, Rect(x + s.w - c.x(82), y + 3, c.x(75), c.y(15)),
            string("v%s%s", VERSION_STR, SA_DEBUG_SUFFIX));

        y += c.y(6) + s.h;
        saveAsDefault = widget::Ctor<Button>(this, 
            Rect(x, y, c.x(100), c.y(23)), "Save As Default");
        saveAsDefault->callback.to(this, &This::saveDefaults);

        x += c.x(7) + s.w;
        y += c.y(7) + c.y(23);
        
        // [C4 FIX] Llamadas explícitas a Window para evitar ambigüedad de sobrecarga
        this->Window::size(x, y);
        this->Window::text(NAME " Settings");

        return true;
    }

    void settingsChanged(bool newColors)
    {
        updateSettingsTab();
        updateColorsTab();

        if (newColors)
        {
            scheme->text("");
            ::Settings key(config::colorsKey);
            for (int i = ColorsIndex; i < config::Count; i++)
                key.set(shared.settings.name(i), shared.settings(i));
        }
    }

    LayerBase* addLayer(const char* tag)
    {
        LayerBase* layer = autorelease(new LayerBase);
        app->loadLayer(tag, this, layer);
        tabs->add(tag, (Window*)layer);
        return layer;
    }

    void tabChanged(int value)
    {
        saveAsDefault->visible(!value);
    }

    // Lógica de construcción de Widgets
    static void makeEdit(This* that, Ctor& ctor, int i) {
        that->widget[i].ctor(ctor(i + stepperTag), ctor(i), ctor(i + labelTag));
    }

    static void makeCombo(This* that, Ctor& ctor, int i) {
        Combo master(ctor(i));
        for (int v = 0; v <= that->settings.range(i); v++)
            master->add(that->settings.text(i, v));
        that->widget[i].ctor(&*master, &that->nullWidget, ctor(i + labelTag));
    }

    static void makeToggle(This* that, Ctor& ctor, int i) {
        Toggle master(ctor(i));
        that->widget[i].ctor(&*master, &that->nullWidget, &*master);
    }

    void initSettingsTab()
    {
        using namespace settings;
        struct Entry { Index index; void (*make)(This*, Ctor&, int); };
        static const Entry map[] = {
            {inputChannel, makeCombo}, {peakEnable, makeToggle}, {peakDecay, makeEdit},
            {avrgEnable, makeToggle}, {avrgTime, makeEdit}, {avrgBarType, makeCombo},
            {avrgBarSize, makeEdit}, {holdEnable, makeToggle}, {holdInfinite, makeToggle},
            {holdTime, makeEdit}, {holdDecay, makeEdit}, {holdBarType, makeCombo},
            {holdBarSize, makeEdit}, {levelCeil, makeEdit}, {levelRange, makeEdit},
            {levelGrid, makeEdit}, {freqGridType, makeCombo}, {bandsPerOctave, makeCombo},
            {avrgSlope, makeEdit}
        };

        Ctor ctor(addLayer("Settings"));
        for (int i = 0; i < (int)(sizeof(map) / sizeof(Entry)); i++)
            map[i].make(this, ctor, map[i].index);

        for (int i = 0; i < SettingsCount; i++) {
            widget[i].range(settings.range(i));
            widget[i].label(settings.label(i));
            widget[i].callback.to(this, &This::valueChanged, i);
            widget[i].textCallback.to(this, &This::textChanged, i);
        }
        updateSettingsTab();
    }

    void updateSettingsTab() {
        for (int i = 0; i < SettingsCount; i++) {
            widget[i].value(settings.value(i));
            valueChanged(settings.value(i), i);
        }
    }

    void valueChanged(int value, int index) {
        settings.value(index, value);
        widget[index].text(settings.text(index));
        applyDependencies(index);
    }

    void textChanged(int, int index) {
        int v = settings.value(index);
        settings.value(index, widget[index].text()());
        if (v != settings.value(index))
            widget[index].value(settings.value(index));
    }

    void applyDependencies(int index) {
        if (settings::depended[index].use) {
            int value = 0;
            for (int i = 0; i < SettingsCount; i++)
                value += !!settings.value(i) << i;

            for (int i = 0; i < SettingsCount; i++)
                widget[i].enable(settings::depended[i].value == (value & settings::depended[i].mask));
        }
    }

    void saveDefaults() {
        ::Settings key(config::prefsKey);
        for (int i = 0; i < SettingsCount; i++)
            key.set(shared.settings.name(i), shared.settings(i));
    }

    // GESTIÓN DE COLORES
    void initColorsTab(Font::Scale& c)
    {
        LayerBase* layer = addLayer("Colors");
        Ctor ctor(layer);

        for (int i = 0; i < ColorsCount; i++) {
            color[i] = ctor(i + colorTag);
            color[i]->callback.to(this, &This::colorChanged, i);
            AnyWidget edit(ctor(i));
            edit->enable(*settings.unit(i + ColorsIndex) == 'a');
            opacity[i].ctor(ctor(i + stepperTag), edit, ctor(i + labelTag));
            opacity[i].range(100);
            opacity[i].label(settings.label(i + ColorsIndex));
            opacity[i].callback.to(this, &This::colorChanged, i);
            opacity[i].textCallback.to(this, &This::opacityTextChanged, i);
        }

        scheme = ctor(400);
        scheme->callback.to(this, &This::loadColorScheme);
        
        Rect r(scheme->position(), scheme->size());
        r.x += r.w + c.x(8);
        r.y -= 1; r.h += 2;
        
        Toolbar toolbar = widget::Ctor<Toolbar>(layer, r);
        toolbar->callback.to(this, &This::saveColorScheme);
        toolbar->add(2, "toolbar-off", "toolbar-on");

        updateColorsTab();
        loadCustomColors();
    }

    void updateColorsTab() {
        int index = 0;
        string name;
        string current = scheme->text();
        scheme->clear();
        ::Settings key(config::colorsKey);
        while (*(name = key.subKey(index++)))
            scheme->add(name);
        scheme->text(current());

        for (int i = 0; i < ColorsCount; i++) {
            color[i]->value(shared.settings(i + ColorsIndex));
            opacity[i].value(argb2opacity(shared.settings(i + ColorsIndex)));
            colorUpdate(i, true);
        }
    }

    void loadColorScheme(int value) {
        const string& name = scheme->text();
        ::Settings key(::Settings(config::colorsKey), name);
        for (int i = ColorsIndex; i < config::Count; i++)
            shared.settings(i, key.get(shared.settings.name(i), shared.settings(i)), false);
        updateColorsTab();
        shared.settings.notify();
    }

    void saveColorScheme(int delete_) {
        Window task;
        const string& name = scheme->text();
        if (!*name) {
            if (!delete_) task.alert(NAME, "Please enter a name first.");
            return;
        }
        ::Settings key(config::colorsKey);
        if (delete_) key.deleteKey(name);
        else {
            ::Settings sub(key, name);
            for (int i = ColorsIndex; i < config::Count; i++)
                sub.set(shared.settings.name(i), shared.settings(i));
        }
        updateColorsTab();
    }

    static int argb2opacity(int v) { return ((v >> 24) & 0xFF) * 100 / 255; }
    static int opacity2argb(int op, int argb) {
        return (argb & 0xFFFFFF) | ((op * 255 / 100) << 24);
    }

    void colorChanged(int, int index) { colorUpdate(index, true); }

    void opacityTextChanged(int, int index) {
        string text = opacity[index].text();
        int v = kali::max(0, kali::min(100, (int)atoi(text())));
        opacity[index].value(v);
        colorUpdate(index, false);
    }

    void colorUpdate(int index, bool updateText) {
        int v = opacity[index].value();
        if (updateText) opacity[index].text(string("%i%%", v));
        shared.settings(index + ColorsIndex, opacity2argb(v, color[index]->value()));
    }

    static void saveCustomColors() {
        ::Settings key(config::colorsKey);
        for (int i = 0; i < 16; i++) {
            char name[3] = {'.', (char)('a' + i), 0};
            key.set(name, (int)ColorWell::custom(i));
        }
    }

    static void loadCustomColors() {
        ::Settings key(config::colorsKey);
        for (int i = 0; i < 16; i++) {
            char name[3] = {'.', (char)('a' + i), 0};
            ColorWell::custom(i) = key.get(name, 0x101010 * i);
        }
    }

    void initPrefsTab(Font::Scale& c) {
        LayerBase* layer = addLayer("Preferences");
        Rect r(c.x(11), c.y(15), c.x(300), c.y(16));
        for (int i = 0; i < PrefCount; i++) {
            pref[i] = widget::Ctor<Toggle>(layer, r, config::prefs[i].label);
            pref[i]->callback.to(this, &This::prefChanged, config::prefs[i].index);
            r.y += c.y(13) * 2;
        }
        updatePrefsTab();
    }

    void updatePrefsTab() {
        ::Settings key(config::prefsKey);
        for (int i = 0; i < PrefCount; i++)
            pref[i]->value(!!key.get(config::PrefName()[i], config::prefs[i].default_));
    }

    void prefChanged(int value, config::PrefIndex index) {
        ::Settings(config::prefsKey).set(config::PrefName()[index], value);
    }

    Editor(Shared& shared) : shared(shared), settings(shared.settings) {}
    virtual ~Editor() {}

private:
    enum {
        SettingsCount = sa::config::ColorsIndex,
        ColorsIndex   = sa::config::ColorsIndex,
        ColorsCount   = sa::config::ColorsCount,
        PrefCount     = sa::config::PrefCount
    };

    sa::Shared&  shared;
    Settings     settings;
    LayerTabs    tabs;
    Button       saveAsDefault;
    Combo        scheme;
    Compound     widget  [SettingsCount];
    ColorWell    color   [ColorsCount];
    Compound     opacity [ColorsCount];
    Toggle       pref    [PrefCount];
    NullWidget   nullWidget;

    enum { stepperTag = 100, labelTag = 200, colorTag = 300, otherTag = 400 };
};

} // ~ namespace sa

#endif
