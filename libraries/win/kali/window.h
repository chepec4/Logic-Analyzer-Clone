#ifndef KALI_WINDOW_INCLUDED
#define KALI_WINDOW_INCLUDED

#include "kali/platform.h"
#include "kali/string.h"

// [C4 ARCHITECTURE FIX]
// Usamos la geometría centralizada en lugar de redefinir tipos POD pobres.
// Esto permite que sa::Display use métodos avanzados como intersects() y unite().
#include "kali/geometry.h"

#include <windows.h>

namespace kali {

// Forward declarations
namespace graphics { struct BufferedContext; }
namespace ui { namespace native { namespace widget { struct Base; } } }

// ============================================================================
// WINDOW WRAPPER
// ============================================================================

struct Window
{
    typedef HWND Handle;
    Handle handle;

    Window(Handle handle = nullptr) : handle(handle) {}
    operator Handle () const { return handle; }

    bool open() {
        if (!handle) return false;
        ::ShowWindow(handle, SW_SHOW);
        ::UpdateWindow(handle);
        return true;
    }

    void close() {
        // En Kali moderno, LayerBase::close gestiona el DestroyWindow.
        // Aquí solo invalidamos el handle local.
        handle = nullptr;
    }

    void invalidate() {
        if (handle) ::InvalidateRect(handle, nullptr, FALSE);
    }

    // Texto / Título
    string text() const {
        char buf[256];
        ::GetWindowTextA(handle, buf, 255);
        return string("%s", buf);
    }
    string title() const { return text(); }
    void title(const char* s) { text(s); }
    void text(const char* s) { ::SetWindowTextA(handle, s); }

    // Geometría usando kali::geometry
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

    // Helpers
    void show(bool v = true) { ::ShowWindow(handle, v ? SW_SHOW : SW_HIDE); }
    bool visible() const { return ::IsWindowVisible(handle); }
    void enable(bool v = true) { ::EnableWindow(handle, v); }
    bool enabled() const { return ::IsWindowEnabled(handle); }
};

} // ~ namespace kali

#endif // KALI_WINDOW_INCLUDED
