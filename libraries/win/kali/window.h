#ifndef KALI_WINDOW_INCLUDED
#define KALI_WINDOW_INCLUDED

#include "kali/platform.h"
#include "kali/string.h"

#include <windows.h>

namespace kali {

// [C4 MASTER SYNC] Declaraciones adelantadas de infraestructura gráfica
namespace graphics { struct BufferedContext; }
namespace ui { namespace native { namespace widget { struct Base; } } }

// Definición de las plantillas base de Traits para que sa.resizer.h pueda especializarlas
namespace details {
    template <typename T> struct Dispatch;
    template <typename T> struct Traits;
    template <typename T> struct TraitsBase;
}

// ============================================================================
// GEOMETRÍA NATIVA
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
// CLASE WINDOW (WRAPPER Win32)
// ============================================================================

struct Window
{
    typedef HWND Handle;
    Handle handle;

    Window(Handle h = nullptr) : handle(h) {}
    operator Handle () const { return handle; }

    template <typename T>
    T* object() const {
        return reinterpret_cast<T*>(::GetWindowLongPtr(handle, GWLP_USERDATA));
    }

    void object(void* ptr) {
        ::SetWindowLongPtr(handle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(ptr));
    }

    void invalidate(bool erase = false) const { 
        if (handle) ::InvalidateRect(handle, nullptr, erase ? TRUE : FALSE); 
    }
    
    void update() const { if (handle) ::UpdateWindow(handle); }
    void show(int cmd = SW_SHOW) const { if (handle) ::ShowWindow(handle, cmd); }
    bool visible() const { return handle && (::IsWindowVisible(handle) != FALSE); }

    void text(const char* s) { if (handle) ::SetWindowTextA(handle, s ? s : ""); }
    string text() const {
        char buf[512];
        if (handle) ::GetWindowTextA(handle, buf, 511);
        else buf[0] = 0;
        return string("%s", buf);
    }

    Point position() const { RECT r = {0}; if (handle) ::GetWindowRect(handle, &r); return Point(r.left, r.top); }
    Size size() const { RECT r = {0}; if (handle) ::GetClientRect(handle, &r); return Size(r.right, r.bottom); }

    void position(int x, int y) {
        if (handle) ::SetWindowPos(handle, nullptr, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
    }

    void size(int w, int h) {
        if (handle) ::SetWindowPos(handle, nullptr, 0, 0, w, h, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
    }

    void size(Size s) { size(s.w, s.h); }

    void centerAt(const Window* parent = nullptr) {
        Rect target;
        if (!parent) target = Rect(0, 0, ::GetSystemMetrics(SM_CXSCREEN), ::GetSystemMetrics(SM_CYSCREEN));
        else target = Rect(parent->position(), parent->size());
        Size mySize = size();
        position(target.x + (target.w - mySize.w) / 2, target.y + (target.h - mySize.h) / 2);
    }
};

} // namespace kali

#endif
