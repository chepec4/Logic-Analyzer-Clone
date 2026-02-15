#ifndef KALI_WINDOW_INCLUDED
#define KALI_WINDOW_INCLUDED

#include "kali/platform.h"
#include "kali/string.h"
#include <windows.h>

namespace kali {

// ============================================================================
// 1. GEOMETRÍA UNIFICADA (Resuelve conflictos con geometry.h)
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
    
    // Constructor estándar
    Rect(int x = 0, int y = 0, int w = 0, int h = 0) : x(x), y(y), w(w), h(h) {}
    
    // [C4 FIX] Constructor por Point y Size (Requerido por sa.editor.h)
    Rect(Point p, Size s) : x(p.x), y(p.y), w(s.w), h(s.h) {}
    
    // [C4 FIX] Constructor por Size (Requerido por sa.display.h)
    Rect(Size s) : x(0), y(0), w(s.w), h(s.h) {}

    int right() const  { return x + w; }
    int bottom() const { return y + h; }
    bool empty() const { return w <= 0 || h <= 0; }
    
    bool contains(Point p) const {
        return p.x >= x && p.x < (x + w) && p.y >= y && p.y < (y + h);
    }

    // [C4 FIX] Operador de comparación (Requerido por sa.resizer.h)
    bool operator!=(const Rect& r) const {
        return x != r.x || y != r.y || w != r.w || h != r.h;
    }
    bool operator==(const Rect& r) const { return !(*this != r); }
};

// Utilidad global de pantalla
inline Size screenSize() { 
    return Size(GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)); 
}

// ============================================================================
// 2. ESTRUCTURA WINDOW (Win32 Wrapper Optimizado)
// ============================================================================

struct Window
{
    typedef HWND Handle;
    Handle handle;

    Window(Handle h = 0) : handle(h) {}
    operator Handle () const { return handle; }

    // Gestión de Punteros de Clase (Thunks/UserData)
    template <typename T> T* object() const {
        return (T*) ::GetWindowLongPtr(handle, GWLP_USERDATA);
    }
    template <typename T> void object(T* ptr) {
        ::SetWindowLongPtr(handle, GWLP_USERDATA, (LONG_PTR)ptr);
    }

    // Comandos de Dibujo
    void invalidate(bool erase = false) const { ::InvalidateRect(handle, 0, erase); }
    void update() const { ::UpdateWindow(handle); }

    // Sistema de Alertas compatible con C++14
    bool alert(const char* txt, const char* cap = "Message", const char* comm = 0) const {
        string s = comm ? string("%s\n%s", txt, comm) : string("%s", txt);
        return IDOK == ::MessageBox(handle, s(), cap, MB_OK | MB_ICONINFORMATION | MB_TASKMODAL);
    }

    // Gestión de Propiedades
    void show(int c = SW_SHOW) const { ::ShowWindow(handle, c); }
    void hide() const { ::ShowWindow(handle, SW_HIDE); }
    bool visible() const { return !!::IsWindowVisible(handle); }
    void text(const char* s) { ::SetWindowText(handle, s); }
    
    string text() const {
        char buf[512];
        ::GetWindowText(handle, buf, 512);
        return string(buf);
    }

    // [C4 FIX] El plugin llama a title() como getter
    string title() const { return text(); }
    
    // [C4 FIX] El plugin llama a title(const char*) como setter
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

    // Sobrecarga por componentes
    void size(int w, int h) {
        ::SetWindowPos(handle, 0, 0, 0, w, h, SWP_NOMOVE | SWP_NOZORDER | SWP_NOREDRAW);
    }

    // [C4 FIX] Sobrecarga por objeto Size (Requerido por sa.display.h)
    void size(Size s) {
        size(s.w, s.h);
    }
};

} // ~ namespace kali

#endif
