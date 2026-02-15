#ifndef KALI_UI_NATIVE_WIDGETS_INCLUDED
#define KALI_UI_NATIVE_WIDGETS_INCLUDED

#include <windows.h>
#include <commctrl.h>
#include "kali/window.h"
#include "kali/ui/native/widgets.base.h"

namespace kali {

// ============================================================================
// 1. UTILIDADES DE SISTEMA
// ============================================================================

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
    using kali::Timer;
    using kali::WaitCursor;

// ============================================================================
// 2. FUENTES Y ESCALADO (Vital para sa.editor.h)
// ============================================================================

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
        ::SystemParametersInfo(SPI_GETNONCLIENTMETRICS, 0, &ncm, 0);
        handle = ::CreateFontIndirect(&ncm.lfMessageFont);
    }
    ~Font() { ::DeleteObject(handle); }
};

// ============================================================================
// 3. NAMESPACE WIDGET (Implementación Real de Contratos)
// ============================================================================

namespace widget {

struct Base : Interface {
    HWND handle;
    Base() : handle(nullptr) {}

    // [C4 ARCHITECTURE] Implementación de virtuals de Interface (ui/base.h)
    virtual bool enable() const override { return handle && ::IsWindowEnabled(handle); }
    virtual void enable(bool e) override { if(handle) ::EnableWindow(handle, e); }
    virtual bool visible() const override { return handle && ::IsWindowVisible(handle); }
    virtual void visible(bool v) override { if(handle) ::ShowWindow(handle, v ? SW_SHOW : SW_HIDE); }
    
    virtual int  value() const override { return 0; }
    virtual void value(int) override {}
    virtual int  range() const override { return 100; }
    virtual void range(int) override {}

    virtual void destroy() override { if(handle) ::DestroyWindow(handle); handle = nullptr; }
    virtual Window::Handle expose() const override { return handle; }

    virtual void text(const char* s) override { if(handle) ::SetWindowTextA(handle, s ? s : ""); }
    virtual string text() const override {
        if (!handle) return string();
        char buf[512]; ::GetWindowTextA(handle, buf, 512);
        // [C4 FIX] Usamos sprintf para evitar el constructor privado String(T)
        return string("%s", buf);
    }

    void ctor(Parent* p, HWND h) { handle = h; Window(h).object(this); }
    Size size() { RECT r; ::GetClientRect(handle, &r); return Size(r.right, r.bottom); }
    
    static void thunk_(HWND h, int msg) {
        Base* w = Window(h).object<Base>();
        if (w) w->callback(msg);
    }
    
    virtual void draw(DRAWITEMSTRUCT*) {}
    enum { style_ = 0, styleEx_ = 0 };
};

// --- Widgets Estándar ---
struct Text : Base { static const char* class_() { return WC_STATIC; } };
struct TextRight : Text { enum { style_ = SS_RIGHT }; };
struct Edit : Base { static const char* class_() { return WC_EDIT; } };
struct TextCopy : Edit { enum { style_ = ES_READONLY }; };
struct Button : Base { static const char* class_() { return WC_BUTTON; } };
struct Toggle : Button {
    int  value() const override { return (int)::SendMessage(handle, BM_GETCHECK, 0, 0); }
    void value(int v) override { ::SendMessage(handle, BM_SETCHECK, v, 0); }
};

struct Combo : Base {
    static const char* class_() { return WC_COMBOBOX; }
    void add(const char* t) { ::SendMessage(handle, CB_ADDSTRING, 0, (LPARAM)t); }
    void clear() { ::SendMessage(handle, CB_RESETCONTENT, 0, 0); }
    int  value() const override { return (int)::SendMessage(handle, CB_GETCURSEL, 0, 0); }
    void value(int v) override { ::SendMessage(handle, CB_SETCURSEL, v, 0); }
};

// --- Widgets Específicos del Analizador ---
struct Meter : Base {
    static const char* class_() { return PROGRESS_CLASS; }
    void range(int maxV) override { ::SendMessage(handle, PBM_SETRANGE32, 0, maxV); }
    void value(int v) override { ::SendMessage(handle, PBM_SETPOS, v, 0); }
};

struct Break : Text { enum { style_ = SS_ETCHEDHORZ }; };

struct LayerTabs : Base {
    static const char* class_() { return WC_TABCONTROL; }
    int add(const char* t, Window* win) {
        TCITEM ti = { TCIF_TEXT | TCIF_PARAM, 0, 0, const_cast<char*>(t), 0, (LPARAM)win->handle };
        return (int)::SendMessage(handle, TCM_INSERTITEM, 0, (LPARAM)&ti);
    }
};

struct Toolbar : Base { 
    static const char* class_() { return TOOLBARCLASSNAME; }
    void add(int n, const char* a, const char* b=nullptr) {} 
};

struct ColorWell : Base {
    static COLORREF& custom(int i) { static COLORREF c[16]; return c[i]; }
};

// --- Constructores de Recursos ---
struct ResourceCtor {
    Parent* p;
    ResourceCtor(Parent* p) : p(p) {}
    struct Aux {
        HWND h; Parent* p;
        Aux(Parent* p, int id) : p(p) { h = ::GetDlgItem(p->window()->handle, id); }
        template <typename T> operator Ptr<T>() const {
            T* w = new T(); w->ctor(p, h);
            p->attach(w); return w;
        }
    };
    Aux operator()(int id) const { return Aux(p, id); }
};

// Constructor Dinámico compatible con Ptr<T>
template <typename T> inline
typename T::Type* Ctor(Parent* parent, const Rect& r, const char* text = nullptr) {
    typedef typename T::Type W;
    HWND h = ::CreateWindowExA(W::styleEx_, W::class_(), text,
        WS_CHILD | WS_VISIBLE | W::style_,
        r.x, r.y, r.w, r.h, parent->window()->handle, nullptr, GetModuleHandle(nullptr), nullptr);
    if (h) {
        W* w = new W(); w->ctor(parent, h);
        parent->attach(w); return w;
    }
    return nullptr;
}

} // ~ widget
}} // ~ native / ui
} // ~ kali

#endif
