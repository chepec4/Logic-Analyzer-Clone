#ifndef KALI_UI_NATIVE_INCLUDED
#define KALI_UI_NATIVE_INCLUDED

#include "kali/ui/native/widgets.base.h"
#include "kali/ui/native/widgets.h" 

namespace kali   {
namespace ui      {
namespace native {

// ............................................................................
// 1. TYPEDEFS (Smart Pointers para UI)
// ............................................................................

typedef Ptr <widget::Interface> AnyWidget;
typedef Ptr <widget::Button>    Button;
typedef Ptr <widget::Combo>     Combo;
typedef Ptr <widget::Text>      Text;
typedef Ptr <widget::Text>      TextRight;
typedef Ptr <widget::Text>      Label;
typedef Ptr <widget::Edit>      Edit;
typedef Ptr <widget::Toggle>    Toggle;
typedef Ptr <widget::Toolbar>   Toolbar;
typedef Ptr <widget::ColorWell> ColorWell;
typedef Ptr <widget::Stepper>   Stepper;
typedef Ptr <widget::Fader>     Fader;

// ............................................................................
// 2. WINDOW BASE (Sin APIs Obsoletas)
// ............................................................................

struct WindowBase : Window
{
    enum { UsesGraphics = 1, DropShadow = 0, SysCaption = 0 };
    virtual ~WindowBase() {}

    // [C4 FIX] centerAt usando geometría pura (sin rect() obsoleto)
    void centerAt(const Window* at = nullptr)
    {
        Rect r = (!at || !at->handle) 
            ? Rect(screenSize()) 
            : Rect(at->position(), at->size());
        
        Size s = this->size();
        this->position(r.x + (r.w - s.w) / 2, r.y + (r.h - s.h) / 2);
    }
    
    void clientSize(int w, int h)
    {
        RECT r = {0, 0, w, h};
        ::AdjustWindowRectEx(&r, 
            ::GetWindowLong(handle, GWL_STYLE), 
            FALSE, 
            ::GetWindowLong(handle, GWL_EXSTYLE));
        this->size(r.right - r.left, r.bottom - r.top);
    }
    
    // Exponer métodos base para evitar ambigüedades
    using Window::size;
    using Window::position;
    using Window::text;

    static Size screenSize() {
        return Size(::GetSystemMetrics(SM_CXSCREEN), ::GetSystemMetrics(SM_CYSCREEN));
    }
};

// ............................................................................
// 3. LAYER BASE (Gestión de Lifecycle)
// ............................................................................

struct LayerBase : WindowBase, widget::Parent
{
    enum { isLayer = true };
    
    Window* window() override { return this; }
    
    // [C4 FIX] Firma correcta para Kali Moderno (objeto, owner)
    void attach(ReleaseAny* obj) override {
        if (obj) app->autorelease.add(obj, static_cast<ReleaseAny*>(nullptr));
    }

    virtual bool open() { return true; }
    
    // [C4 FIX] close() reemplaza a destroy()
    virtual void close() {
        if (handle) {
            ::DestroyWindow(handle);
            handle = nullptr;
        }
    }
    
    virtual ~LayerBase() { close(); }
};

// ............................................................................
// 4. LAYER TABS (Implementación Nativa)
// ............................................................................

struct LayerTabs : widget::Base
{
    typedef LayerTabs Type; // Requerido por factory
    static const char* class_() { return WC_TABCONTROL; }

    void add(const char* name, Window* window)
    {
        TCITEM ti = {0};
        ti.mask = TCIF_TEXT | TCIF_PARAM;
        ti.pszText = const_cast<char*>(name);
        ti.lParam = reinterpret_cast<LPARAM>(window ? window->handle : 0);

        int count = (int)::SendMessage(handle, TCM_GETITEMCOUNT, 0, 0);
        ::SendMessage(handle, TCM_INSERTITEM, count, reinterpret_cast<LPARAM>(&ti));

        if (window && window->handle) {
            ::SetParent(window->handle, handle);
            RECT r; ::GetClientRect(handle, &r);
            ::MoveWindow(window->handle, 2, 25, r.right - 4, r.bottom - 27, TRUE);
        }
    }
    
    Size size() const {
        RECT r; ::GetClientRect(handle, &r);
        return Size(r.right, r.bottom);
    }
};

typedef Ptr<LayerTabs> LayerTabsPtr;

} // ~ native
} // ~ ui
} // ~ kali

#endif
