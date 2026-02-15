#ifndef KALI_WINDOW_INCLUDED
#define KALI_WINDOW_INCLUDED

#include "kali/platform.h"
#include "kali/string.h"
#include <windows.h>

// [C4 FIX] Bloqueamos la redefinición en geometry.h para evitar "conflicting declaration"
#ifndef KALI_GEOMETRY_OVERRIDE
#define KALI_GEOMETRY_OVERRIDE
#endif

namespace kali {

// Forward declarations para satisfacer las plantillas de app.details.h
namespace graphics { struct BufferedContext; }
namespace ui { namespace native { namespace widget { struct Base; } } }

// ============================================================================
// 1. GEOMETRÍA UNIFICADA (Estructuras POD compatibles con Win32)
// ============================================================================

struct Point {
    int x, y;
    Point(int x = 0, int y = 0) : x(x), y(y) {}
};

struct Size {
    int w, h;
    Size(int w = 0, int h = 0) : w(w), h(h) {}
};

struct Rect {
    int x, y, w, h;
    
    // Constructores requeridos por el plugin y sa.editor.h
    Rect(int x = 0, int y = 0, int w = 0, int h = 0) : x(x), y(y), w(w), h(h) {}
    Rect(Point p, Size s) : x(p.x), y(p.y), w(s.w), h(s.h) {}
    Rect(Size s) : x(0), y(0), w(s.w), h(s.h) {}

    int right() const  { return x + w; }
    int bottom() const { return y + h; }
    bool empty() const { return w <= 0 || h <= 0; }
    
    bool contains(Point p) const {
        return p.x >= x && p.x < (x + w) && p.y >= y && p.y < (y + h);
    }

    // Operadores de comparación requeridos por sa.resizer.h
    bool operator!=(const Rect& r) const {
        return x != r.x || y != r.y || w != r.w || h != r.h;
    }
    bool operator==(const Rect& r) const { return !(*this != r); }
};

// Utilidad de pantalla
inline Size screenSize() { 
    return Size(GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)); 
}

// ============================================================================
// 2. ESTRUCTURA WINDOW (Win32 API Wrapper)
// ============================================================================

struct Window
{
    typedef HWND Handle;
    Handle handle;

    Window(Handle h = 0) : handle(h) {}
    operator Handle () const { return handle; }

    // Gestión de Punteros de Clase (Uso de GWLP_USERDATA para evitar colisiones en x64)
    template <typename T> T* object() const {
        return (T*) ::GetWindowLongPtr(handle, GWLP_USERDATA);
    }
    template <typename T> void object(T* ptr) {
        ::SetWindowLongPtr(handle, GWLP_USERDATA, (LONG_PTR)ptr);
    }

    // Métodos de actualización
    void invalidate(bool erase = false) const { ::InvalidateRect(handle, 0, erase); }
    void update() const { ::UpdateWindow(handle); }

    // Sistema de Alertas
    bool alert(const char* txt, const char* cap = "Message", const char* comm = 0) const {
        string s;
        if (comm) s = string("%s\n%s", txt, comm);
        else s = string("%s", txt);
        return IDOK == ::MessageBox(handle, s(), cap, MB_OK | MB_ICONINFORMATION | MB_TASKMODAL);
    }

    // Propiedades de Ventana
    void show(int c = SW_SHOW) const { ::ShowWindow(handle, c); }
    void hide() const { ::ShowWindow(handle, SW_HIDE); }
    bool visible() const { return !!::IsWindowVisible(handle); }
    
    void text(const char* s) { ::SetWindowText(handle, s); }
    string text() const {
        string s;
        ::GetWindowText(handle, s(), s.size);
        return s;
    }

    // [C4 FIX] Compatibilidad con la interfaz de sa.editor.h
    string title() const { return text(); }
    void title(const char* s) { text(s); }

    // Geometría Dinámica
    Point position() const {
        RECT r; ::GetWindowRect(handle, &r);
        return Point(r.left, r.top);
    }

    void position(int x, int y) {
        ::SetWindowPos(handle, 0, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOREDRAW);
    }

    Size size() const {
        RECT r; ::GetClientRect(handle, &r);
        return Size(r.right, r.bottom);
    }

    // Sobrecargas de dimensionamiento
    void size(int w, int h) {
        ::SetWindowPos(handle, 0, 0, 0, w, h, SWP_NOMOVE | SWP_NOZORDER | SWP_NOREDRAW);
    }

    void size(Size s) {
        size(s.w, s.h);
    }
    
    // Para centrar ventanas (Requerido por kali/ui/native.h)
    void centerAt(const Window* at = 0) {
        Rect r = !at ? Rect(screenSize()) : Rect(at->position(), at->size());
        Size s = size();
        position(r.x + (r.w - s.w) / 2, r.y + (r.h - s.h) / 2);
    }
};

} // ~ namespace kali

#endif
