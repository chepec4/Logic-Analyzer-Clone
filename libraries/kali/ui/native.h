#ifndef KALI_UI_NATIVE_INCLUDED
#define KALI_UI_NATIVE_INCLUDED

#include "kali/ui/native/widgets.base.h"
#include "kali/ui/native/widgets.h" 

namespace kali   {
namespace ui      {
namespace native {

// ............................................................................
// 1. TYPEDEFS (Punteros Inteligentes para Widgets)
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
// 2. WINDOW BASE (Base Nativa Win32)
// ............................................................................

struct WindowBase : Window
{
    enum { UsesGraphics = 1, DropShadow = 0, SysCaption = 0 };
    virtual ~WindowBase() {}

    // Centrado de ventana usando aritmética de Rect/Size (Sin APIs obsoletas)
    void centerAt(const Window* at = nullptr)
    {
        Rect r = (!at || !at->handle) 
            ? Rect(screenSize()) 
            : Rect(at->position(), at->size());

        Size s = this->size();
        this->position(r.x + (r.w - s.w) / 2, r.y + (r.h - s.h) / 2);
    }
    
    // Exponer métodos de la clase base Window para evitar ocultamiento
    Size size() const { return Window::size(); }
    using Window::size;
    using Window::position;
    using Window::text;

    static Size screenSize() {
        return Size(::GetSystemMetrics(SM_CXSCREEN), ::GetSystemMetrics(SM_CYSCREEN));
    }
};

// ............................................................................
// 3. LAYER BASE (Gestión de Ciclo de Vida y Memoria)
// ............................................................................

struct LayerBase : WindowBase, widget::Parent
{
    enum { isLayer = true };
    Window* window() override { return this; }
    
    // [C4 FIX] Corrección de Ownership para Kali Containers 2026
    // El método 'add' es privado en AutoReleasePool<void>.
    // La interfaz pública correcta es el operador functor 'operator()'.
    void attach(ReleaseAny* obj) override {
        if (obj) app->autorelease(obj);
    }

    virtual bool open() { return true; }
    
    // Destrucción segura usando API nativa
    virtual void close() {
        if (handle) {
            ::DestroyWindow(handle);
            handle = nullptr;
        }
    }
    
    virtual ~LayerBase() { close(); }
};

// ............................................................................
// 4. LAYER TABS (Control de Pestañas Nativo)
// ............................................................................

struct LayerTabs : widget::Base
{
    typedef LayerTabs Type;
    static const char* class_() { return WC_TABCONTROL; }

    void add(const char* name, Window* window)
    {
        TCITEM ti = {0};
        ti.mask = TCIF_TEXT | TCIF_PARAM;
        ti.pszText = const_cast<char*>(name);
        ti.lParam = reinterpret_cast<LPARAM>(window ? window->handle : 0);

        int count = (int)::SendMessage(handle, TCM_GETITEMCOUNT, 0, 0);
        ::SendMessage(handle, TCM_INSERTITEM, count, reinterpret_cast<LPARAM>(&ti));

        // Incrustar la ventana hija dentro del área cliente del Tab
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
