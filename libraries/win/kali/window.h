#ifndef KALI_WINDOW_INCLUDED
#define KALI_WINDOW_INCLUDED

#include "kali/platform.h"
#include "kali/string.h"

// [C4 ARCHITECTURE] Bloqueo preventivo de la colisión de geometry.h
#ifndef KALI_GEOMETRY_INCLUDED
#define KALI_GEOMETRY_INCLUDED
#endif

#include <windows.h>

namespace kali {

// Forward declarations para app.details.h
namespace graphics { struct BufferedContext; }
namespace ui { namespace native { namespace widget { struct Base; } } }

// ============================================================================
// GEOMETRÍA POD (Sincronizada con el motor nativo)
// ============================================================================

struct Point { int x, y; Point(int x=0, int y=0):x(x),y(y){} };
struct Size  { int w, h; Size(int w=0, int h=0):w(w),h(h){} };
struct Rect  { 
    int x, y, w, h; 
    Rect(int x=0, int y=0, int w=0, int h=0):x(x),y(y),w(w),h(h){}
    Rect(Point p, Size s):x(p.x),y(p.y),w(s.w),h(s.h){}
    Rect(Size s):x(0),y(0),w(s.w),h(s.h){}
    bool operator!=(const Rect& r) const { return x!=r.x || y!=r.y || w!=r.w || h!=r.h; }
};

// ============================================================================
// WINDOW WRAPPER — RECONSTRUIDO PARA GCC 12.2
// ============================================================================

struct Window
{
    typedef HWND Handle;
    Handle handle;

    Window(Handle h = 0) : handle(h) {}
    operator Handle () const { return handle; }

    // Acceso seguro a USERDATA para x64
    template <typename T>
    T* object() const {
        return reinterpret_cast<T*>(::GetWindowLongPtr(handle, GWLP_USERDATA));
    }

    template <typename T>
    void object(T* ptr) {
        ::SetWindowLongPtr(handle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(ptr));
    }

    void invalidate(bool erase = false) const { ::InvalidateRect(handle, nullptr, erase); }
    void update() const { ::UpdateWindow(handle); }
    void show(int cmd = SW_SHOW) const { ::ShowWindow(handle, cmd); }
    void hide() const { ::ShowWindow(handle, SW_HIDE); }
    bool visible() const { return ::IsWindowVisible(handle) != FALSE; }

    // TEXT SAFE — Mikhailov Design Compliance
    void text(const char* s) { ::SetWindowTextA(handle, s ? s : ""); }

    string text() const {
        char buf[512];
        ::GetWindowTextA(handle, buf, 512);
        // [C4 FIX] Usar sprintf para saltar el constructor privado de String(T)
        return string("%s", buf);
    }

    string title() const { return text(); }
    void title(const char* s) { text(s); }

    Point position() const {
        RECT r; ::GetWindowRect(handle, &r);
        return Point(r.left, r.top);
    }

    void position(int x, int y) {
        ::SetWindowPos(handle, nullptr, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
    }

    Size size() const {
        RECT r; ::GetClientRect(handle, &r);
        return Size(r.right, r.bottom);
    }

    void size(int w, int h) {
        ::SetWindowPos(handle, nullptr, 0, 0, w, h, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
    }

    void size(Size s) { size(s.w, s.h); }

    // Lógica de centrado heredada del plugin original
    void centerAt(const Window* parent = nullptr) {
        Rect target;
        if (!parent) {
            target = Rect(0, 0, ::GetSystemMetrics(SM_CXSCREEN), ::GetSystemMetrics(SM_CYSCREEN));
        } else {
            target = Rect(parent->position(), parent->size());
        }
        Size mySize = size();
        position(target.x + (target.w - mySize.w) / 2, target.y + (target.h - mySize.h) / 2);
    }

    bool alert(const char* txt, const char* cap = "Message", const char* comm = nullptr) const {
        string s = comm ? string("%s\n%s", txt, comm) : string("%s", txt);
        return IDOK == ::MessageBoxA(handle, s(), cap, MB_OK | MB_ICONINFORMATION | MB_TASKMODAL);
    }
};

} // namespace kali

#endif
