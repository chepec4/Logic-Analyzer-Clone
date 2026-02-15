#ifndef KALI_UI_NATIVE_INCLUDED
#define KALI_UI_NATIVE_INCLUDED

#include "kali/ui/native/widgets.base.h"
#include "kali/ui/native/widgets.h" 

namespace kali   {
namespace ui     {
namespace native {

// ............................................................................
// Typedefs para punteros inteligentes (UI API)
// ............................................................................

typedef Ptr <widget::Interface> AnyWidget;
typedef Ptr <widget::Button>    Button;
typedef Ptr <widget::Combo>     Combo;
typedef Ptr <widget::Text>      Text;
typedef Ptr <widget::Text>      TextRight;
typedef Ptr <widget::Text>      Label;
typedef Ptr <widget::Edit>      Edit;
typedef Ptr <widget::Group>     Group;
typedef Ptr <widget::Table>     Table;
typedef Ptr <widget::Tree>      Tree;
typedef Ptr <widget::Tab>       Tab;
typedef Ptr <widget::Toggle>    Toggle;
typedef Ptr <widget::Toolbar>   Toolbar;
typedef Ptr <widget::ColorWell> ColorWell;
typedef Ptr <widget::Stepper>   Stepper;
typedef Ptr <widget::Fader>     Fader;

// ............................................................................
// Clases Base para Ventanas y Capas
// ............................................................................

struct WindowBase : Window
{
    // [FIX] Defaults para TraitsBase en app.details.h
    enum 
    { 
        UsesGraphics = 0, 
        DropShadow   = 0, 
        SysCaption   = 0 
    };

    virtual ~WindowBase() {}

    void alert(const char* title, const char* text) const
    {
        MessageBox(handle, text, title, MB_ICONEXCLAMATION | MB_OK);
    }
    
    static Size screenSize() {
        return Size(GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
    }

    void centerAt(const Window* at)
    {
        // [FIX] Lógica simplificada para evitar error 'no member named rect' en const
        RECT rc;
        if (at && at->handle) {
            ::GetWindowRect(at->handle, &rc);
        } else {
            rc.left = 0; rc.top = 0;
            rc.right = GetSystemMetrics(SM_CXSCREEN);
            rc.bottom = GetSystemMetrics(SM_CYSCREEN);
        }
        
        Rect r(rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top);
        Rect w = rect(); // mi propio rect
        position(r.x + (r.w - w.w) / 2, r.y + (r.h - w.h) / 2);
    }
    
    void clientSize(int w, int h)
    {
        RECT r = {0, 0, w, h};
        AdjustWindowRectEx(&r, GetWindowLong(handle, GWL_STYLE), FALSE, GetWindowLong(handle, GWL_EXSTYLE));
        size(r.right - r.left, r.bottom - r.top);
    }
    
    // Sobrecargas para exponer métodos de Window protegidos o ambigüos
    void size(int w, int h) { Window::size(w, h); }
    Size size() const { return Window::rect().size(); }
    void text(const char* t) { Window::text(t); }
};

struct LayerBase : WindowBase
{
    enum { isLayer = true };
    
    // [FIX] Métodos requeridos por DispatchLoaded en app.details.h
    virtual bool open() { return true; }
    virtual void close() {}
    
    virtual ~LayerBase() {}
};

struct LayerTabs : widget::Tab
{
    // Tipo auxiliar para ResourceCtor
    typedef widget::Tab Type; 

    void add(const char* name, Window* window)
    {
        // Llamada a la base
        widget::Tab::add(name);
        
        if (window && window->handle) {
            window->show(false);
            ::SetParent(window->handle, handle);
            
            // Ajustar tamaño
            RECT r; 
            ::GetClientRect(handle, &r);
            ::MoveWindow(window->handle, 0, 24, r.right, r.bottom - 24, TRUE);
        }
    }
    
    // Helper para C4
    Size size() { 
        RECT r; ::GetClientRect(handle, &r); 
        return Size(r.right, r.bottom); 
    }
};

} // ~ namespace native
} // ~ namespace ui
} // ~ namespace kali

#endif // ~ KALI_UI_NATIVE_INCLUDED
