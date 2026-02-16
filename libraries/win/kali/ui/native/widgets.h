#ifndef KALI_UI_NATIVE_WIDGETS_INCLUDED
#define KALI_UI_NATIVE_WIDGETS_INCLUDED

#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include "kali/window.h"
#include "kali/ui/native/widgets.base.h"

namespace kali {
namespace ui { namespace native {

struct Font : ReleaseAny {
    struct Scale {
        enum { refx = 6, refy = 13 };
        int x_, y_;
        Scale(int x, int y) : x_(x), y_(y) {}
        int x(int v) { return (v * x_ + refx / 2) / refx; }
        int y(int v) { return (v * y_) / refy; }
    };
    static const Font& main() {
        static Font* aux = nullptr;
        if (!aux) aux = app->autorelease(new Font);
        return *aux;
    }
    HFONT handle;
    operator HFONT() const { return handle; }
    Scale scale() const {
        HDC dc = ::CreateCompatibleDC(nullptr);
        HGDIOBJ f = ::SelectObject(dc, handle);
        TEXTMETRIC tm; ::GetTextMetrics(dc, &tm);
        ::SelectObject(dc, f); ::DeleteDC(dc);
        return Scale(tm.tmAveCharWidth, tm.tmHeight);
    }
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

    virtual bool enable() const override { return handle && ::IsWindowEnabled(handle); }
    virtual void enable(bool e) override { if(handle) ::EnableWindow(handle, e); }
    virtual bool visible() const override { return handle && ::IsWindowVisible(handle); }
    virtual void visible(bool v) override { if(handle) ::ShowWindow(handle, v ? SW_SHOW : SW_HIDE); }
    virtual int  value() const override { return 0; }
    virtual void value(int) override {}
    virtual int  range() const override { return 100; }
    virtual void range(int) override {}
    virtual void text(const char* s) override { if(handle) ::SetWindowTextA(handle, s ? s : ""); }
    virtual string text() const override {
        if (!handle) return string();
        char buf[512]; ::GetWindowTextA(handle, buf, 512);
        return string("%s", buf);
    }
    virtual Window::Handle expose() const override { return handle; }
    void destroy() { if(handle) ::DestroyWindow(handle); handle = nullptr; }

    void ctor(const Window* p, HWND h) { 
        handle = h; 
        Window(h).object(this); 
        ::SendMessage(handle, WM_SETFONT, (WPARAM)(HFONT)Font::main(), 0);
    }
    
    static void thunk_(HWND h, int msg) {
        Base* w = Window(h).object<Base>();
        if (w) w->callback(msg);
    }
    virtual void draw(DRAWITEMSTRUCT*) {}
    static const char* class_() { return "Button"; }
    enum { style_ = 0, styleEx_ = 0 };
};

struct Text : Base { typedef Text Type; static const char* class_() { return "Static"; } };
struct Edit : Base { typedef Edit Type; static const char* class_() { return "Edit"; } };
struct Button : Base { typedef Button Type; static const char* class_() { return "Button"; } };
struct Toggle : Button {
    int  value() const override { return (int)::SendMessage(handle, BM_GETCHECK, 0, 0); }
    void value(int v) override { ::SendMessage(handle, BM_SETCHECK, v, 0); }
};
struct Combo : Base {
    typedef Combo Type;
    static const char* class_() { return "ComboBox"; }
    void add(const char* t) { ::SendMessage(handle, CB_ADDSTRING, 0, (LPARAM)t); }
    int  value() const override { return (int)::SendMessage(handle, CB_GETCURSEL, 0, 0); }
    void value(int v) override { ::SendMessage(handle, CB_SETCURSEL, v, 0); }
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
    const DWORD style = WS_CHILD | WS_VISIBLE | W::style_;
    HWND h = ::CreateWindowExA(0, W::class_(), text, style,
        r.x, r.y, r.w, r.h, (HWND)parent->handle, nullptr, GetModuleHandle(nullptr), nullptr);
    if (h) {
        W* w = new W(); w->ctor(parent, h);
        return w;
    }
    return nullptr;
}

} // ~ widget
}} // ~ native / ui
} // ~ kali

#endif // KALI_UI_NATIVE_WIDGETS_INCLUDED
