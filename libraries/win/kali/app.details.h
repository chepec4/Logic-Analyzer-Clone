#ifndef KALI_APP_DETAILS_INCLUDED
#define KALI_APP_DETAILS_INCLUDED

#include "kali/window.h"
#include "kali/ui/native/widgets.h"
#include "kali/graphics.h"

namespace kali {

struct AppDetails : ReleaseAny {
    Module* module_;
    Window* mainWindow_;
    bool    graphics_;
    AppDetails() : module_(0), mainWindow_(0), graphics_(false) {}
};

namespace details {

// [C4 MASTER FIX] Declaración de la plantilla primaria antes de especializar
template <typename T, bool Graphics>
struct Dispatch;

template <typename T>
struct Dispatch <T, false> {
    static void initializer() { app->initGraphics(); }
    static LRESULT CALLBACK thunk(HWND handle, UINT msg, WPARAM w, LPARAM l) {
        T* window = Window(handle).object<T>();
        if (msg == WM_NCCREATE && l) {
            window = (T*) ((CREATESTRUCT*) l)->lpCreateParams;
            window->handle = handle;
            window->object(window);
        }
        return window ? dispatch(window, msg, w, l) : ::DefWindowProc(handle, msg, w, l);
    }
    static LRESULT dispatch(T* window, UINT msg, WPARAM w, LPARAM l) {
        switch (msg) {
        case WM_PAINT: {
            PAINTSTRUCT ps; HDC dc = ::BeginPaint(window->handle, &ps);
            Rect r(ps.rcPaint.left, ps.rcPaint.top, ps.rcPaint.right-ps.rcPaint.left, ps.rcPaint.bottom-ps.rcPaint.top);
            graphics::BufferedContext c(dc, r); 
            window->draw(c);
            ::EndPaint(window->handle, &ps); return 0;
        }
        case WM_DESTROY: window->close(); return 0;
        }
        return ::DefWindowProc(window->handle, msg, w, l);
    }
};

// [C4 MASTER FIX] Declaración de la plantilla primaria de Traits
template <typename T>
struct TraitsBase : Dispatch <T, !T::UsesGraphics> {
    enum { classStyle = CS_DBLCLKS, styleEx = WS_EX_CONTROLPARENT, style = WS_CHILD | WS_VISIBLE };
    static const char* name() { return "KaliWindow"; }
    static bool create(const Window* parent, T* window) {
        WNDCLASSEX wcx = { sizeof(wcx) };
        wcx.style         = classStyle;
        wcx.hInstance     = app->module();
        wcx.lpfnWndProc   = thunk;
        wcx.lpszClassName = name();
        wcx.hCursor       = ::LoadCursor(nullptr, IDC_ARROW);
        ::RegisterClassEx(&wcx);

        HWND h = ::CreateWindowEx((DWORD)styleEx, name(), name(), (DWORD)style,
            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 
            parent ? (HWND)parent->handle : nullptr, nullptr, app->module(), window);
        return !!h;
    }
};

template <typename T> struct Traits : TraitsBase <T> {};

} // namespace details

template <typename T>
bool app::createWindow(const Window* parent, T* window) { return details::Traits<T>::create(parent, window); }

template <typename T>
bool app::createLayer(const Window* parent, T* window) { return details::Traits<T>::create(parent, window); }

} // namespace kali

#endif
