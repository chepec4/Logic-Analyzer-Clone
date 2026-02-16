#ifndef KALI_UI_NATIVE_INCLUDED
#define KALI_UI_NATIVE_INCLUDED

#include "kali/ui/base.h"
#include "kali/ui/native/widgets.h"

namespace kali {
namespace ui {
namespace native {

// ............................................................................
// 1. TYPEDEFS DE PUNTEROS INTELIGENTES (Modelo Actual)
// ............................................................................

typedef Ptr<widget::Interface> AnyWidget;
typedef Ptr<widget::Button>    Button;
typedef Ptr<widget::Combo>     Combo;
typedef Ptr<widget::Text>      Text;
typedef Ptr<widget::Edit>      Edit;
typedef Ptr<widget::Toggle>    Toggle;
typedef Ptr<widget::Toolbar>   Toolbar;
typedef Ptr<widget::ColorWell> ColorWell;
typedef Ptr<widget::Stepper>   Stepper;
typedef Ptr<widget::Fader>     Fader;

// ............................................................................
// 2. CLASES BASE DE VENTANAS
// ............................................................................

struct WindowBase : Window
{
    // [C4 ARCHITECTURE] Flags requeridos por Traits en app.details.h
    enum { UsesGraphics = 1, DropShadow = 0, SysCaption = 0 };

    virtual ~WindowBase() {}

    /**
     * [C4 FIX] REEMPLAZO DE centerAt()
     * Eliminamos la llamada a rect() (obsoleto). 
     * Usamos la geometría pura definida en window.h.
     */
    void centerAt(const Window* at = nullptr)
    {
        // Obtenemos el rectángulo de referencia (Monitor o Padre)
        Rect r = (!at || !at->handle) 
            ? Rect(screenSize()) 
            : Rect(at->position(), at->size());

        // Obtenemos nuestro propio tamaño (vía base Window)
        Size s = this->size();

        // Calculamos posición centrada y aplicamos
        this->position(r.x + (r.w - s.w) / 2, r.y + (r.h - s.h) / 2);
    }

    /**
     * [C4 FIX] REEMPLAZO DE size()
     * Eliminamos Window::rect().size() por ser API eliminada.
     */
    Size size() const { return Window::size(); }
    
    void clientSize(int w, int h)
    {
        RECT r = {0, 0, w, h};
        ::AdjustWindowRectEx(&r, 
            ::GetWindowLong(handle, GWL_STYLE), 
            FALSE, 
            ::GetWindowLong(handle, GWL_EXSTYLE));
        this->size(r.right - r.left, r.bottom - r.top);
    }

    // Sincronización de métodos base
    using Window::size;
    using Window::position;
    using Window::text;
};

// ............................................................................
// 3. CAPAS Y JERARQUÍA DE UI
// ............................................................................

struct LayerBase : WindowBase, widget::Parent
{
    enum { isLayer = true };

    // Implementación de widget::Parent (Contrato obligatorio)
    Window* window() override { return this; }
    
    /**
     * [C4 FIX] GESTIÓN DE OWNERSHIP
     * Se usa el pool de autorelease de la aplicación global.
     */
    void attach(ReleaseAny* obj) override {
        if (obj) app->autorelease.add(obj);
    }

    virtual bool open() { return true; }
    virtual void close() {
        if (handle) {
            ::DestroyWindow(handle);
            handle = nullptr;
        }
    }

    virtual ~LayerBase() { close(); }
};

// ............................................................................
// 4. SISTEMA DE PESTAÑAS (Mapeo Nativo)
// ............................................................................

struct LayerTabs : widget::Base
{
    // [C4 FIX] Type obligatorio para widget::Ctor
    typedef LayerTabs Type;
    static const char* class_() { return WC_TABCONTROL; }

    /**
     * [C4 FIX] IMPLEMENTACIÓN DE add()
     * El modelo actual no permite stubs; realizamos la inserción real en Win32.
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
            // Ajuste de área de cliente para el contenido de la pestaña
            RECT r; ::GetClientRect(handle, &r);
            ::MoveWindow(window->handle, 2, 25, r.right - 4, r.bottom - 27, TRUE);
            window->show(count == 0); // Solo mostramos la primera por defecto
        }
    }

    Size size() {
        RECT r; ::GetClientRect(handle, &r);
        return Size(r.right, r.bottom);
    }
};

// Typedef para el plugin (C4 compatibility)
typedef Ptr<LayerTabs> LayerTabsPtr;

} // ~ native
} // ~ ui
} // ~ kali

#endif
