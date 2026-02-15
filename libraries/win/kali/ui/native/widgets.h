#ifndef KALI_UI_NATIVE_WIDGETS_INCLUDED
#define KALI_UI_NATIVE_WIDGETS_INCLUDED

#include <windows.h>
#include <commctrl.h>
#include <stdio.h> // Necesario para sprintf en implementación de text()
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
// 2. FUENTES Y ESCALADO (Infraestructura de renderizado)
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
        ::SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(ncm), &ncm, 0);
        handle = ::CreateFontIndirect(&ncm.lfMessageFont);
    }
    ~Font() { ::DeleteObject(handle); }
};

// ============================================================================
// 3. NAMESPACE WIDGET (Implementación Definitiva)
// ============================================================================

namespace widget {

// Base concreta que implementa la Interfaz abstracta
struct Base : Interface {
    HWND handle;
    Base() : handle(nullptr) {}

    // --- Implementación de Interface (ui/base.h) ---
    // [FIX INFRAESTRUCTURA] Override explícito para cumplir contrato abstracto
    
    virtual bool enable() const override { return handle && ::IsWindowEnabled(handle); }
    virtual void enable(bool e) override { if(handle) ::EnableWindow(handle, e); }
    
    virtual bool visible() const override { return handle && ::IsWindowVisible(handle); }
    virtual void visible(bool v) override { if(handle) ::ShowWindow(handle, v ? SW_SHOW : SW_HIDE); }
    
    virtual int  value() const override { return 0; }
    virtual void value(int) override {}
    
    virtual int  range() const override { return 100; }
    virtual void range(int) override {}

    // [FIX INFRAESTRUCTURA] Implementación de text() con buffer seguro
    virtual void text(const char* s) override { if(handle) ::SetWindowTextA(handle, s ? s : ""); }
    virtual string text() const override {
        if (!handle) return string();
        char buf[512]; 
        ::GetWindowTextA(handle, buf, 512);
        return string("%s", buf); // string(T) suele ser privado, usamos format
    }

    virtual Window::Handle expose() const override { return handle; }

    // [FIX INFRAESTRUCTURA] 'destroy' eliminado de la interfaz base en Kali moderno.
    // Lo mantenemos como método helper local (sin override) para limpieza manual si fuera necesario.
    void destroy() { if(handle) ::DestroyWindow(handle); handle = nullptr; }

    // --- Fontanería interna de Windows ---
    
    void ctor(const Window* p, HWND h) { 
        handle = h; 
        Window(h).object(this); 
        ::SendMessage(handle, WM_SETFONT, (WPARAM)(HFONT)Font::main(), 0);
    }
    
    Size size() { RECT r; ::GetClientRect(handle, &r); return Size(r.right, r.bottom); }
    
    static void thunk_(HWND h, int msg) {
        Base* w = Window(h).object<Base>();
        if (w) w->callback(msg);
    }
    
    static void drawThunk_(DRAWITEMSTRUCT* dis) {
        Base* w = Window(dis->hwndItem).object<Base>();
        if (w) w->draw(dis);
    }
    
    virtual void draw(DRAWITEMSTRUCT*) {}
    
    static const char* class_() { return "kali_widget_class"; }
    enum { style_ = 0, styleEx_ = 0 };
};

// --- Widgets Estándar ---
struct Text : Base { static const char* class_() { return "Static"; } };
struct TextRight : Text { enum { style_ = SS_RIGHT }; }; // SS_RIGHT defined in windows.h
struct Edit : Base { static const char* class_() { return "Edit"; } };
struct TextCopy : Edit { enum { style_ = ES_READONLY }; }; // ES_READONLY defined in windows.h
struct Button : Base { static const char* class_() { return "Button"; } };

struct Toggle : Button {
    int  value() const override { return (int)::SendMessage(handle, BM_GETCHECK, 0, 0); }
    void value(int v) override { ::SendMessage(handle, BM_SETCHECK, v, 0); }
};

struct Combo : Base {
    static const char* class_() { return "ComboBox"; }
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

struct Break : Text { enum { style_ = SS_ETCHEDHORZ }; }; // SS_ETCHEDHORZ

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
    // Helper estático usado por sa.editor.h para guardar configuración
    static COLORREF& custom(int i) { static COLORREF c[16]; return c[i]; }
};

// [FIX INFRAESTRUCTURA] Definición de tipos faltantes para sa.editor.h
struct Stepper : Base {}; 
struct Fader : Base {};

// --- Constructores de Recursos (ResourceCtor) ---
// Maneja la vinculación de controles creados en dialogs.rc con objetos C++

struct ResourceCtor {
    const Window* p;
    ResourceCtor(const Window* p) : p(p) {}

    struct Aux {
        HWND h; 
        const Window* p;
        Aux(const Window* p, int id) : p(p) { 
            h = ::GetDlgItem(p->handle, id); 
        }

        // Caso Genérico: Instancia el widget concreto (Button, Combo, etc.)
        template <typename T> 
        operator Ptr<T>() const {
            if (!h) return nullptr;
            T* w = new T(); 
            w->ctor(p, h);
            return w; // Ptr<T> toma ownership
        }

        // [FIX INFRAESTRUCTURA] Caso Crítico: AnyWidget (Ptr<Interface>)
        // Cuando sa.editor.h pide un AnyWidget, no podemos hacer 'new Interface'.
        // Instanciamos 'Base' que es concreto y compatible.
        operator AnyWidget() const {
            if (!h) return nullptr;
            Base* w = new Base();
            w->ctor(p, h);
            return Ptr<Interface>(w);
        }
    };

    Aux operator()(int id) const { return Aux(p, id); }
};

// --- Constructor Dinámico (Ctor) ---
// Usado para crear widgets que no están en el RC (ej. Tabs, Labels dinámicos)

template <typename T> 
inline typename T::Type* Ctor(const Window* parent, const Rect& r, const char* text = nullptr) 
{
    // T::Type es el tipo real del widget (ej. LayerTabs) extraído del Ptr<T>
    typedef typename T::Type W; 
    
    // Fallback seguro si W::style_ no existe (SFINAE simplificado)
    const DWORD style = WS_CHILD | WS_VISIBLE | W::style_;
    
    HWND h = ::CreateWindowExA(
        0, // styleEx
        W::class_(), 
        text,
        style,
        r.x, r.y, r.w, r.h, 
        parent->handle, 
        nullptr, 
        GetModuleHandle(nullptr), 
        nullptr
    );

    if (h) {
        W* w = new W(); 
        w->ctor(parent, h);
        return w;
    }
    return nullptr;
}

} // ~ widget
}} // ~ native / ui
} // ~ kali

#endif
