#ifndef KALI_UI_NATIVE_WIDGETS_INCLUDED
#define KALI_UI_NATIVE_WIDGETS_INCLUDED

#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include "kali/window.h"
#include "kali/ui/native/widgets.base.h"
#include "kali/ui/native/details.h"

namespace kali {

// ----------------------------------------------------------------------------
// UTILITIES
// ----------------------------------------------------------------------------

struct Timer : UsesCallback {
    Window* w;
    Timer() : w(nullptr) {}
    void start(Window* win, unsigned ms) {
        stop(); w = win;
        if (w) ::SetTimer(w->handle, (UINT_PTR)this, ms, thunk);
    }
    void stop() {
        if (w) ::KillTimer(w->handle, (UINT_PTR)this);
        w = nullptr;
    }
    static VOID CALLBACK thunk(HWND, UINT, UINT_PTR p, DWORD) {
        Timer* t = reinterpret_cast<Timer*>(p);
        if (t) t->callback(0);
    }
};

struct WaitCursor {
    HCURSOR old;
    explicit WaitCursor(bool active = true) {
        old = ::SetCursor(::LoadCursor(nullptr, active ? IDC_WAIT : IDC_ARROW));
    }
    ~WaitCursor() { ::SetCursor(old); }
};

namespace ui { namespace native {

// ----------------------------------------------------------------------------
// WIDGET IMPLEMENTATION
// ----------------------------------------------------------------------------

namespace widget {

// Base que fusiona la funcionalidad de Window nativa con la Interfaz de Kali
struct Base : Window, Interface
{
    // Interface kali::ui::widget::Interface
    bool enable() const override { return Window::enabled(); }
    void enable(bool v) override { Window::enable(v); }
    bool visible() const override { return Window::visible(); }
    void visible(bool v) override { Window::show(v); }
    
    int  value() const override { return 0; }
    void value(int v) override {}

    // Interface kali::ui::native::widget::Interface
    int  range() const override { return 0; }
    void range(int v)  override {}

    string text() const override { return Window::text(); }
    void text(const char* s) override { Window::text(s); }

    Window::Handle expose() const override { return handle; }

    // Inicializador post-construcción
    void ctor(const Window* parent, HWND h) {
        handle = h;
    }
    
    virtual ~Base() {
        if (handle) ::DestroyWindow(handle); 
    }
};

// --- CONTROLES ESPECÍFICOS ---

struct Button : Base {
    typedef Button Type;
    static const char* class_() { return "BUTTON"; }
};

struct Toggle : Base {
    typedef Toggle Type;
    static const char* class_() { return "BUTTON"; } // Usar estilo BS_AUTOCHECKBOX
    int value() const override { return (int)::SendMessage(handle, BM_GETCHECK, 0, 0); }
    void value(int v) override { ::SendMessage(handle, BM_SETCHECK, v ? BST_CHECKED : BST_UNCHECKED, 0); }
};

struct Text : Base {
    typedef Text Type;
    static const char* class_() { return "STATIC"; }
};

struct Edit : Base {
    typedef Edit Type;
    static const char* class_() { return "EDIT"; }
};

struct Combo : Base {
    typedef Combo Type;
    static const char* class_() { return "COMBOBOX"; }
    int value() const override { return (int)::SendMessage(handle, CB_GETCURSEL, 0, 0); }
    void value(int v) override { ::SendMessage(handle, CB_SETCURSEL, v, 0); }
    void add(const char* s) { ::SendMessage(handle, CB_ADDSTRING, 0, (LPARAM)s); }
};

struct Toolbar : Base { typedef Toolbar Type; static const char* class_() { return TOOLBARCLASSNAME; } };
struct Stepper : Base { typedef Stepper Type; static const char* class_() { return UPDOWN_CLASS; } };
struct Fader   : Base { typedef Fader   Type; static const char* class_() { return TRACKBAR_CLASS; } };
struct ColorWell : Base { typedef ColorWell Type; static const char* class_() { return "STATIC"; } };

} // namespace widget

// ----------------------------------------------------------------------------
// FACTORIES
// ----------------------------------------------------------------------------

struct ResourceCtor {
    const Window* p;
    ResourceCtor(const Window* p) : p(p) {}
    
    struct Aux {
        HWND h; const Window* p;
        Aux(const Window* p, int id) : p(p) { 
            h = ::GetDlgItem(p->handle, id); 
        }
        
        // Conversión a Ptr tipado (ej: Ptr<Button>)
        template <typename T> operator Ptr<T>() const {
            if (!h) return nullptr;
            T* w = new T(); 
            // Hack seguro: T hereda de widget::Base
            ((widget::Base*)w)->ctor(p, h);
            return w;
        }
        
        // [C4 FIX] Conversión explícita a AnyWidget (Ptr<Interface>)
        operator AnyWidget() const {
            if (!h) return nullptr;
            widget::Base* w = new widget::Base();
            w->ctor(p, h);
            return AnyWidget(w);
        }
    };
    
    Aux operator()(int id) const { return Aux(p, id); }
};

// Constructor dinámico
template <typename T> 
inline typename T::Type* Ctor(const Window* parent, const Rect& r, const char* text = nullptr) 
{
    typedef typename T::Type W;
    DWORD style = WS_CHILD | WS_VISIBLE | WS_TABSTOP; // Estilos base
    
    HWND h = ::CreateWindowExA(
        0, 
        W::class_(), 
        text ? text : "", 
        style, 
        r.x, r.y, r.w, r.h, 
        parent->handle, 
        nullptr, 
        ::GetModuleHandle(nullptr), 
        nullptr
    );
    
    if (!h) return nullptr;
    
    W* w = new W();
    ((widget::Base*)w)->ctor(parent, h);
    
    // Asignar fuente del sistema
    HGDIOBJ font = ::GetStockObject(DEFAULT_GUI_FONT);
    ::SendMessage(h, WM_SETFONT, (WPARAM)font, 0);
    
    return w;
}

} // namespace native
} // namespace ui
} // namespace kali

#endif
