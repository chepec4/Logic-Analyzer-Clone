#ifndef KALI_UI_NATIVE_WIDGETS_INCLUDED
#define KALI_UI_NATIVE_WIDGETS_INCLUDED

#include <windows.h>
#include <commctrl.h>
#include "kali/window.h"
#include "kali/ui/native/widgets.base.h"

namespace kali {

struct Timer : UsesCallback {
    Window* w;
    Timer() : w(nullptr) {}
    void start(Window* win, unsigned ms) {
        stop(); w = win;
        if (w) ::SetTimer(w->handle, (UINT_PTR)this, ms, thunk);
    }
    void stop() { if (w) ::KillTimer(w->handle, (UINT_PTR)this); w = nullptr; }
    static VOID CALLBACK thunk(HWND, UINT, UINT_PTR p, DWORD) {
        Timer* t = reinterpret_cast<Timer*>(p);
        if (t) t->callback(0);
    }
};

namespace ui { namespace native {

// AnyWidget es un alias para el puntero inteligente a la interfaz del widget
using AnyWidget = Ptr<widget::Interface>;

namespace widget {

struct Base : Interface {
    typedef Base Type;
    HWND handle;
    Base() : handle(nullptr) {}

    bool enable() const override { return handle && ::IsWindowEnabled(handle); }
    void enable(bool e) override { if(handle) ::EnableWindow(handle, e ? TRUE : FALSE); }
    bool visible() const override { return handle && ::IsWindowVisible(handle); }
    void visible(bool v) override { if(handle) ::ShowWindow(handle, v ? SW_SHOW : SW_HIDE); }
    int  value() const override { return 0; }
    void value(int) override {}
    int  range() const override { return 100; }
    void range(int) override {}
    void text(const char* s) override { if(handle) ::SetWindowTextA(handle, s ? s : ""); }
    string text() const override {
        char buf[512] = {0};
        if (handle) ::GetWindowTextA(handle, buf, 511);
        return string("%s", buf);
    }
    Window::Handle expose() const override { return handle; }

    void ctor(const Window* p, HWND h) { 
        handle = h; 
        Window(h).object(this); 
    }
    
    static const char* class_() { return "Button"; }
};

struct ResourceCtor {
    const Window* p;
    ResourceCtor(const Window* p) : p(p) {}
    struct Aux {
        HWND h; const Window* p;
        Aux(const Window* p, int id) : p(p) { h = ::GetDlgItem(p->handle, id); }
        template <typename T> operator Ptr<T>() const {
            if (!h) return nullptr;
            T* w = new T(); w->ctor(p, h);
            return w;
        }
        // Soluciona: error: expected type-specifier before AnyWidget
        operator AnyWidget() const {
            if (!h) return nullptr;
            Base* w = new Base(); w->ctor(p, h);
            return AnyWidget(w);
        }
    };
    Aux operator()(int id) const { return Aux(p, id); }
};

} // ~ widget
}} // ~ native / ui
} // ~ kali

#endif
