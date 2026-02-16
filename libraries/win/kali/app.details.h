#ifndef KALI_APP_DETAILS_INCLUDED
#define KALI_APP_DETAILS_INCLUDED

#include <windows.h>
#include <commctrl.h>
#include "kali/window.h"
#include "kali/ui/native/widgets.h"
#include "kali/resources.h"
#include "kali/graphics.h"

namespace kali {

struct AppDetails : ReleaseAny {
    Module* module_;
    Window* mainWindow_;
    bool    graphics_;
    AppDetails() : module_(0), mainWindow_(0), graphics_(false) {}
};

inline Module* app::module()     const {return details_->module_;}
inline Window* app::mainWindow() const {return details_->mainWindow_;}
inline void    app::useThreads()       {}
inline void    app::initGraphics() {
#if defined KALI_GRAPHICS_INCLUDED
    if (!details_->graphics_) {
        autorelease(new graphics::Initializer);
        details_->graphics_ = true;
    }
#endif
}

namespace details {

template <typename Traits, typename T>
bool createWindow(const Window* parent, T* window) {
    WNDCLASSEX wcx = { sizeof(wcx) };
    wcx.style         = Traits::classStyle;
    wcx.hInstance     = app->module();
    wcx.lpfnWndProc   = Traits::thunk;
    wcx.lpszClassName = Traits::name();
    wcx.hCursor       = ::LoadCursor(0, IDC_ARROW);
    wcx.hbrBackground = ::GetSysColorBrush(COLOR_3DFACE);
    
    ::RegisterClassEx(&wcx);
    
    // [C4 MASTER FIX] Cast explícito del error para evitar colisión con objetos temporales
    if ((long)::GetLastError() != (long)ERROR_CLASS_ALREADY_EXISTS) { }

    HWND handle = ::CreateWindowEx((DWORD) Traits::styleEx,
        Traits::name(), Traits::name(), (DWORD) Traits::style,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 
        parent ? (HWND)parent->handle : 0, 0, app->module(), window);
    
    return !!handle;
}

template <typename T, bool IsNative> struct Dispatch;

template <typename T>
struct Dispatch <T, false> {
    static void initializer() { app->initGraphics(); }
    
    static LRESULT CALLBACK thunk(HWND handle, UINT msg, WPARAM wparam, LPARAM lparam) {
        T* window = nullptr;
        if (msg == WM_NCCREATE && lparam) {
            window = (T*) ((CREATESTRUCT*) lparam)->lpCreateParams;
            window->handle = handle;
            window->object(window);
        } else {
            window = Window(handle).object<T>();
        }
        return window ? dispatch(window, msg, wparam, lparam) : ::DefWindowProc(handle, msg, wparam, lparam);
    }

    static LRESULT dispatch(T* window, UINT msg, WPARAM wparam, LPARAM lparam) {
        switch (msg) {
        case WM_PAINT: {
            PAINTSTRUCT ps; 
            HDC dc = ::BeginPaint(window->handle, &ps);
            Rect r(ps.rcPaint.left, ps.rcPaint.top, ps.rcPaint.right-ps.rcPaint.left, ps.rcPaint.bottom-ps.rcPaint.top);
            graphics::BufferedContext c(dc, r); 
            window->draw(c);
            ::EndPaint(window->handle, &ps); 
            return 0;
        }
        case WM_DESTROY: 
            window->close(); 
            return 0;
        }
        return ::DefWindowProc(window->handle, msg, wparam, lparam);
    }
};

template <typename T>
struct TraitsBase : Dispatch <T, !T::UsesGraphics> {
    enum {
        classStyle = CS_DBLCLKS,
        styleEx    = WS_EX_CONTROLPARENT,
        style      = WS_CHILD | WS_VISIBLE
    };
    static const char* name() { return "KaliWindow"; }
    static bool create(const Window* parent, T* window) { return createWindow<TraitsBase>(parent, window); }
};

template <typename T> struct Traits : TraitsBase <T> {};
template <typename T> struct LayerTraits : Traits <T> {};

} // ~ namespace details

template <typename T>
bool app::createWindow(const Window* parent, T* window) { return details::Traits<T>::create(parent, window); }

template <typename T>
bool app::createLayer(const Window* parent, T* window) { return details::LayerTraits<T>::create(parent, window); }

} // ~ namespace kali

#endif // ~ KALI_APP_DETAILS_INCLUDED
