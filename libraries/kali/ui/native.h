#ifndef KALI_UI_NATIVE_INCLUDED
#define KALI_UI_NATIVE_INCLUDED

#include "kali/ui/native/widgets.base.h"
#include "kali/ui/native/widgets.h" // Incluir implementación win32

// ............................................................................

namespace kali   {
namespace ui     {
namespace native {

// ............................................................................
// [FIX INFRAESTRUCTURA] Definiciones de compatibilidad para tipos Legacy
// ............................................................................
namespace widget {
    // Definimos stubs para widgets que pudieron ser renombrados o eliminados
    // para que Ptr<T> pueda compilar.
    struct Stepper : Interface {
        // Implementación dummy para satisfacer interfaz abstracta si se instancia
        bool enable() const override { return false; }
        void enable(bool) override {}
        bool visible() const override { return false; }
        void visible(bool) override {}
        int  value() const override { return 0; }
        void value(int) override {}
        int  range() const override { return 0; }
        void range(int) override {}
        string text() const override { return ""; }
        void text(const char*) override {}
        Window::Handle expose() const override { return 0; }
    };
    
    struct Fader : Interface {
        bool enable() const override { return false; }
        void enable(bool) override {}
        bool visible() const override { return false; }
        void visible(bool) override {}
        int  value() const override { return 0; }
        void value(int) override {}
        int  range() const override { return 0; }
        void range(int) override {}
        string text() const override { return ""; }
        void text(const char*) override {}
        Window::Handle expose() const override { return 0; }
    };
}
// ............................................................................

typedef Ptr <widget::Interface> AnyWidget;
typedef Ptr <widget::Button>    Button;
typedef Ptr <widget::Combo>     Combo;
typedef Ptr <widget::Text>      Text;
typedef Ptr <widget::Text>      TextRight; // Alias
typedef Ptr <widget::Text>      Label;     // Alias
typedef Ptr <widget::Edit>      Edit;
typedef Ptr <widget::Group>     Group;
typedef Ptr <widget::Table>     Table;
typedef Ptr <widget::Tree>      Tree;
typedef Ptr <widget::Tab>       Tab;
typedef Ptr <widget::Toggle>    Toggle;
typedef Ptr <widget::Toolbar>   Toolbar;
typedef Ptr <widget::ColorWell> ColorWell;

// Tipos restaurados
typedef Ptr <widget::Stepper>   Stepper;
typedef Ptr <widget::Fader>     Fader;

// ............................................................................

struct WindowBase : Window
{
    virtual ~WindowBase() {}

    void alert(const char* title, const char* text) const
    {
        MessageBox(handle, text, title, MB_ICONEXCLAMATION | MB_OK);
    }
    
    // [FIX INFRAESTRUCTURA] Añadida función helper faltante
    static Size screenSize() {
        return Size(GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
    }

    void centerAt(const Window* at)
    {
        Rect r = !at ? Rect(0, 0, screenSize().w, screenSize().h) // Corregido constructor
                     : at->rect();
        Rect w = rect();
        position(r.x + (r.w - w.w) / 2, r.y + (r.h - w.h) / 2);
    }
    
    void clientSize(int w, int h)
    {
        RECT r = {0, 0, w, h};
        AdjustWindowRectEx(&r, GetWindowLong(handle, GWL_STYLE), FALSE, GetWindowLong(handle, GWL_EXSTYLE));
        size(r.right - r.left, r.bottom - r.top);
    }
    
    void size(int w, int h)
    {
        Window::size(w, h);
    }
    
    void text(const char* t)
    {
        Window::text(t);
    }
};

struct LayerBase : WindowBase
{
    enum { isLayer = true };
    // [FIX] destroy() eliminado, se usa destructor virtual
    virtual ~LayerBase() {}
};

struct LayerTabs : widget::Tab
{
    void add(const char* name, Window* window)
    {
        widget::Tab::add(name);
        window->show(false);
        // Hack para adjuntar ventana hija
        SetParent(window->handle, handle);
        MoveWindow(window->handle, 0, 24, size().w, size().h - 24, TRUE);
    }
};

// ............................................................................

} // ~ namespace native
} // ~ namespace ui
} // ~ namespace kali

#endif // ~ KALI_UI_NATIVE_INCLUDED
