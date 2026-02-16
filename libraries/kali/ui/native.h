#ifndef KALI_UI_NATIVE_INCLUDED
#define KALI_UI_NATIVE_INCLUDED

#include "kali/ui/native/widgets.base.h"
#include "kali/ui/native/widgets.h" 

namespace kali   {
namespace ui      {
namespace native {

// ............................................................................
// 1. TYPEDEFS PARA PUNTEROS INTELIGENTES (UI API MODERNA)
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
// 2. CLASE BASE PARA VENTANAS (SIN APIS OBSOLETAS)
// ............................................................................

struct WindowBase : Window
{
    enum { UsesGraphics = 1, DropShadow = 0, SysCaption = 0 };

    virtual ~WindowBase() {}

    /**
     * [C4 ARCHITECTURE FIX] centerAt
     * Eliminamos la llamada al inexistente rect().
     * Usamos la geometría pura de la base Window.
     */
    void centerAt(const Window* at = nullptr)
    {
        // Rectángulo de destino (Pantalla o ventana padre)
        Rect r = (!at || !at->handle) 
            ? Rect(screenSize()) 
            : Rect(at->position(), at->size());

        // Tamaño actual usando el método de la clase base Window
        Size s = this->size();

        // Cálculo de posición centrada sin métodos auxiliares eliminados
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
    
    // Exponemos explícitamente los métodos de la base nativa para evitar ambigüedad
    using Window::size;
    using Window::position;
    using Window::text;

    static Size screenSize() {
        return Size(::GetSystemMetrics(SM_CXSCREEN), ::GetSystemMetrics(SM_CYSCREEN));
    }
};

// ............................................................................
// 3. CAPA BASE (LIFECYCLE MODERNO)
// ............................................................................

struct LayerBase : WindowBase, widget::Parent
{
    enum { isLayer = true };

    // Implementación de Parent requerida por widgets.h
    Window* window() override { return this; }
    
    // [C4 ARCHITECTURE FIX] attach() ahora usa el pool de la aplicación
    void attach(ReleaseAny* obj) override {
        if (obj) app->autorelease.add(obj);
    }

    virtual bool open() { return true; }
    
    // [C4 ARCHITECTURE FIX] Reemplazo de destroy() por close() nativo
    virtual void close() {
        if (handle) {
            ::DestroyWindow(handle);
            handle = nullptr;
        }
    }
    
    virtual ~LayerBase() { close(); }
};

// ............................................................................
// 4. CONTROL DE PESTAÑAS (NATIVO)
// ............................................................................

struct LayerTabs : widget::Base
{
    typedef LayerTabs Type; // Requerido por el factory widget::Ctor

    /**
     * [C4 FIX] add()
     * Inserción real de pestañas Win32 sin depender de stubs antiguos.
     */
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
            // Ajuste de offset para el área de dibujo de la pestaña
            ::MoveWindow(window->handle, 2, 25, r.right - 4, r.bottom - 27, TRUE);
        }
    }
    
    Size size() const {
        RECT r; ::GetClientRect(handle, &r);
        return Size(r.right, r.bottom);
    }
};

// Typedef final para cumplimiento de sa.editor.h
typedef Ptr<LayerTabs> LayerTabsPtr;

} // ~ native
} // ~ ui
} // ~ kali

#endif
