#ifndef KALI_UI_NATIVE_WIDGETS_INCLUDED
#define KALI_UI_NATIVE_WIDGETS_INCLUDED

#include <windows.h>
#include <commctrl.h>
#include "kali/window.h"
#include "kali/ui/native/widgets.base.h"

namespace kali {

// Temporizador nativo para el bombeo de la UI de sa::Display
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

struct Font : ReleaseAny {
    struct Scale {
        int x_, y_;
        Scale(int x, int y) : x_(x), y_(y) {}
        int x(int v) { return (v * x_ + 3) / 6; }
        int y(int v) { return (v * y_) / 13; }
    };
    static const Font& main() {
        static Font* aux = nullptr;
        if (!aux) aux = app->autorelease(new Font);
        return *aux;
    }
    HFONT handle;
    operator HFONT() const { return handle; }
    Scale scale() const { return Scale(8, 16); }
    Font() {
        NONCLIENTMETRICS ncm = { sizeof(ncm) };
        ::SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(ncm), &ncm, 0);
        handle = ::CreateFontIndirect(&ncm.lfMessageFont);
    }
    ~Font() { ::DeleteObject(handle); }
};

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
        ::SendMessage(handle, WM_SETFONT, (WPARAM)(HFONT)Font::main(), 0);
    }
    
    static const char* class_() { return "Button"; }
};

// --- IMPLEMENTACIONES ---
struct Text    : Base { typedef Text Type;    static const char* class_() { return "Static"; } };
struct Edit    : Base { typedef Edit Type;    static const char* class_() { return "Edit"; } };
struct Button  : Base { typedef Button Type;  static const char* class_() { return "Button"; } };
struct Toggle  : Button {
    int  value() const override { return (int)::SendMessage(handle, BM_GETCHECK, 0, 0); }
    void value(int v) override { ::SendMessage(handle, BM_SETCHECK, v ? BST_CHECKED : BST_UNCHECKED, 0); }
};
struct Combo   : Base {
    typedef Combo Type;
    static const char* class_() { return "ComboBox"; }
    int  value() const override { return (int)::SendMessage(handle, CB_GETCURSEL, 0, 0); }
    void value(int v) override { ::SendMessage(handle, CB_SETCURSEL, v, 0); }
    void add(const char* s) { ::SendMessage(handle, CB_ADDSTRING, 0, (LPARAM)s); }
};

// [C4 MASTER FIX] Stubs de tipos requeridos por sa.display.h
struct Toolbar   : Base { typedef Toolbar Type; };
struct ColorWell : Base { typedef ColorWell Type; };
struct Stepper   : Base { typedef Stepper Type; };
struct Fader     : Base { typedef Fader Type; };

// --- FACTORÍAS ---
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
        
        /**
         * [C4 MASTER FIX] Cast explícito para resolver el tipo AnyWidget (Ptr<Interface>)
         */
        operator kali::ui::native::AnyWidget() const {
            if (!h) return nullptr;
            Base* w = new Base(); w->ctor(p, h);
            return kali::ui::native::AnyWidget(w);
        }
    };
    Aux operator()(int id) const { return Aux(p, id); }
};

template <typename T> 
inline typename T::Type* Ctor(const Window* parent, const Rect& r, const char* text = nullptr) 
{
    typedef typename T::Type W; 
    HWND h = ::CreateWindowExA(0, W::class_(), text ? text : "", WS_CHILD | WS_VISIBLE,
        r.x, r.y, r.w, r.h, (HWND)parent->handle, nullptr, GetModuleHandle(nullptr), nullptr);
    if (h) {
        W* w = new W(); w->ctor(parent, h);
        return w;
    }
    return nullptr;
}

} // ~ namespace widget
}} // ~ namespace native / ui
} // ~ namespace kali

#endif // ~ KALI_UI_NATIVE_WIDGETS_INCLUDED
