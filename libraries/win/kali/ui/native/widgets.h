#ifndef KALI_UI_NATIVE_WIDGETS_INCLUDED
#define KALI_UI_NATIVE_WIDGETS_INCLUDED

#include <windows.h>
#include <commctrl.h>
#include "kali/window.h"
#include "kali/ui/native/widgets.base.h"

namespace kali {
namespace ui { namespace native {

// [C4 MASTER FIX] Definición completa de Font para el entorno nativo
struct Font : ReleaseAny {
    struct Scale {
        int x_, y_;
        Scale(int x, int y) : x_(x), y_(y) {}
        int x(int v) const { return (v * x_ + 3) / 6; }
        int y(int v) const { return (v * y_) / 13; }
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

    Window::Handle expose() const override { return handle; }
    bool enable() const override { return handle && ::IsWindowEnabled(handle); }
    void enable(bool e) override { if(handle) ::EnableWindow(handle, e); }
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

    // [C4 MASTER FIX] Exponer tamaño para sa.editor.h
    Size size() const { RECT r = {0}; if (handle) ::GetClientRect(handle, &r); return Size(r.right, r.bottom); }

    void ctor(const Window* p, HWND h) { 
        handle = h; 
        Window(h).object(this); 
    }
    static const char* class_() { return "Button"; }
};

struct Text : Base { typedef Text Type; static const char* class_() { return "Static"; } };
struct Edit : Base { typedef Edit Type; static const char* class_() { return "Edit"; } };
struct Button : Base { typedef Button Type; static const char* class_() { return "Button"; } };
struct Toggle : Button { };
struct Combo : Base { typedef Combo Type; static const char* class_() { return "ComboBox"; } };
struct LayerTabs : Base { typedef LayerTabs Type; static const char* class_() { return WC_TABCONTROLA; } void add(const char*, LayerBase*){} };

template <typename T> 
inline T* Ctor(const Window* parent, const Rect& r, const char* text = "") {
    HWND h = ::CreateWindowExA(0, T::class_(), text, WS_CHILD | WS_VISIBLE,
        r.x, r.y, r.w, r.h, (HWND)parent->handle, nullptr, GetModuleHandle(nullptr), nullptr);
    if (h) {
        T* w = new T(); w->ctor(parent, h);
        return w;
    }
    return nullptr;
}

} // ~ widget
}} // ~ native / ui
} // ~ kali

#endif
