#ifndef KALI_UI_NATIVE_WIDGETS_INCLUDED
#define KALI_UI_NATIVE_WIDGETS_INCLUDED

#include <windows.h>
#include <commctrl.h>
#include "kali/ui/native/widgets.base.h"

namespace kali {

// ============================================================================
// 1. UTILIDADES DE SISTEMA (Timer y Cursor)
// ============================================================================

struct Timer : UsesCallback {
    Window* w;
    Timer() : w(0) {}
    // Thunk para SetTimer de Win32
    void start(Window* win, unsigned ms) {
        stop();
        w = win;
        ::SetTimer(w->handle, (UINT_PTR)this, ms, thunk);
    }
    void stop() {
        if (w) ::KillTimer(w->handle, (UINT_PTR)this);
        w = 0;
    }
    static VOID CALLBACK thunk(HWND, UINT, UINT_PTR p, DWORD) {
        Timer* t = (Timer*)p;
        if (t) t->callback(0);
    }
};

struct WaitCursor {
    HCURSOR old;
    WaitCursor(bool active = true) {
        old = ::SetCursor(::LoadCursor(0, active ? IDC_WAIT : IDC_ARROW));
    }
    ~WaitCursor() { ::SetCursor(old); }
};

namespace ui { namespace native {
    using kali::Timer;
    using kali::WaitCursor;

// ============================================================================
// 2. FUENTES Y ESCALADO
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
        static Font* aux = 0;
        if (!aux) aux = app->autorelease(new Font);
        return *aux;
    }

    HFONT handle;
    operator HFONT() const { return handle; }
    Scale scale() const { return getScale(handle); }

    Font() {
        NONCLIENTMETRICS ncm = { sizeof(ncm) };
        ::SystemParametersInfo(SPI_GETNONCLIENTMETRICS, 0, &ncm, 0);
        handle = ::CreateFontIndirect(&ncm.lfMessageFont);
    }
    ~Font() { ::DeleteObject(handle); }

private:
    static Scale getScale(HFONT h) {
        HDC dc = ::CreateCompatibleDC(0);
        HGDIOBJ f = ::SelectObject(dc, h);
        TEXTMETRIC tm; ::GetTextMetrics(dc, &tm);
        ::SelectObject(dc, f); ::DeleteDC(dc);
        return Scale(tm.tmAveCharWidth, tm.tmHeight);
    }
};

// ============================================================================
// 3. NAMESPACE WIDGET (Componentes Visuales)
// ============================================================================

namespace widget {

struct Base : Interface {
    HWND handle;
    Base() : handle(0) {}

    // [C4 FIX] Requerido por app.details.h
    virtual void destroy() {
        if (handle) ::DestroyWindow(handle);
        handle = 0;
    }

    virtual void ctor(Parent* p, HWND h) {
        handle = h;
        Window(handle).object(this);
    }

    Window::Handle expose() const { return handle; }
    
    void text(const char* s) { ::SetWindowText(handle, s); }
    string text() const {
        char buf[512]; ::GetWindowText(handle, buf, 512);
        return string(buf);
    }

    void size(int w, int h) {
        ::SetWindowPos(handle, 0, 0, 0, w, h, SWP_NOMOVE | SWP_NOZORDER);
    }
    Size size() {
        RECT r; ::GetClientRect(handle, &r);
        return Size(r.right, r.bottom);
    }

    static void thunk_(HWND h, int msg) {
        Base* w = Window(h).object<Base>();
        if (w) w->callback(msg);
    }
    
    static void drawThunk_(DRAWITEMSTRUCT* ds) {
        Base* w = Window(ds->hwndItem).object<Base>();
        if (w) w->draw(ds);
    }

    virtual void draw(DRAWITEMSTRUCT*) {}
    enum { style_ = 0, styleEx_ = 0 };
};

// Definiciones de Widgets Estándar
struct Text : Base { static const char* class_() { return WC_STATIC; } };
struct Edit : Base { static const char* class_() { return WC_EDIT; } };
struct Button : Base { static const char* class_() { return WC_BUTTON; } };
struct Combo : Base { static const char* class_() { return WC_COMBOBOX; } };

// [C4 FIX] Widgets específicos requeridos por el Analizador
struct Meter : Base { 
    static const char* class_() { return PROGRESS_CLASS; } 
    void range(int maxV) { ::SendMessage(handle, PBM_SETRANGE32, 0, maxV); }
    void value(int v) { ::SendMessage(handle, PBM_SETPOS, v, 0); }
};

struct Break : Text { 
    enum { style_ = SS_ETCHEDHORZ }; 
};

struct LayerTabs : Base {
    static const char* class_() { return WC_TABCONTROL; }
    int add(const char* t, Window* win) {
        TCITEM ti = { TCIF_TEXT | TCIF_PARAM, 0, 0, (char*)t, 0, (LPARAM)win->handle };
        return (int)::SendMessage(handle, TCM_INSERTITEM, 0, (LPARAM)&ti);
    }
};

// Constructor desde Recursos (.rc)
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

// Constructor Dinámico (Programático)
template <typename T> inline
typename T::Type* Ctor(Parent* parent, const Rect& r, const char* text = 0) {
    typedef typename T::Type W;
    HWND h = ::CreateWindowEx(W::styleEx_, W::class_(), text,
        WS_CHILD | WS_VISIBLE | W::style_,
        r.x, r.y, r.w, r.h, parent->window()->handle, 0, GetModuleHandle(0), 0);
    if (h) {
        W* w = new W(); w->ctor(parent, h);
        parent->attach(w); return w;
    }
    return 0;
}

} // ~ widget
}} // ~ native / ui
} // ~ kali

#endif
