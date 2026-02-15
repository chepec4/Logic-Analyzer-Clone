#ifndef KALI_APP_DETAILS_INCLUDED
#define KALI_APP_DETAILS_INCLUDED

#include <windows.h>
#include <commctrl.h>
#include "kali/window.h"
#include "kali/ui/native/widgets.h"
#include "kali/resources.h"

namespace kali {

struct AppDetails : ReleaseAny {
    Module* module_;
    Window* mainWindow_;
    bool    graphics_;
    AppDetails() : module_(0), mainWindow_(0), graphics_(0) {}
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

template <typename Traits>
bool preinitWindow() {
    WNDCLASSEX wcx = { sizeof(wcx) };
    wcx.style         = Traits::classStyle;
    wcx.hInstance     = app->module();
    wcx.lpfnWndProc   = Traits::thunk;
    wcx.lpszClassName = Traits::name();
    wcx.hCursor       = ::LoadCursor(0, IDC_ARROW);
    wcx.hbrBackground = ::GetSysColorBrush(COLOR_3DFACE);
    return !!::RegisterClassEx(&wcx);
}

template <typename Traits, typename T>
bool createWindow(const Window* parent, T* window) {
    Traits::initializer();
    preinitWindow<Traits>();
    HWND handle = ::CreateWindowEx((DWORD) Traits::styleEx,
        Traits::name(), Traits::name(), (DWORD) Traits::style,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 
        parent ? parent->handle : 0, 0, app->module(), window);
    if (handle && !::IsWindowVisible(handle)) ::ShowWindow(handle, SW_SHOWDEFAULT);
    return !!handle;
}

template <typename Traits, typename T>
static bool loadWindow(const char* tag, const Window* parent, T* window) {
    resource::Raw <char> rc(RT_DIALOG, tag);
    resource::details::patchDialogFont(rc.data(), rc.size());
    HWND handle = ::CreateDialogIndirectParam(app->module(),
        (DLGTEMPLATE*) rc.data(), parent->handle, Traits::thunk, (LPARAM) window);
    if (handle && !::IsWindowVisible(handle)) ::ShowWindow(handle, SW_SHOWDEFAULT);
    return !!handle;
}

// Dispatchers
template <typename T, bool IsNative> struct Dispatch;

template <typename T>
struct Dispatch <T, false> {
    static void initializer() { app->initGraphics(); }
    static LRESULT CALLBACK thunk(HWND handle, UINT msg, WPARAM wparam, LPARAM lparam) {
        T* window;
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
        case WM_CREATE: return window->open() ? 0 : -1;
        case WM_DESTROY: 
            window->close(); 
            if (window == app->mainWindow()) ::PostQuitMessage(0);
            return 0;
        case WM_PAINT: {
            PAINTSTRUCT ps; HDC dc = ::BeginPaint(window->handle, &ps);
            if (ps.rcPaint.right > ps.rcPaint.left) {
                Rect r(ps.rcPaint.left, ps.rcPaint.top, ps.rcPaint.right-ps.rcPaint.left, ps.rcPaint.bottom-ps.rcPaint.top);
                graphics::BufferedContext c(dc, r); window->draw(c);
            }
            ::EndPaint(window->handle, &ps); return 0;
        }
        }
        return ::DefWindowProc(window->handle, msg, wparam, lparam);
    }
};

template <typename T>
struct Dispatch <T, true> { // For native controls/dialogs
    static void initializer() {}
    static LRESULT CALLBACK thunk(HWND handle, UINT msg, WPARAM wparam, LPARAM lparam) {
        // ... (Similar logic simplified for native)
        return ::DefWindowProc(handle, msg, wparam, lparam);
    }
};

template <typename T>
struct DispatchLoaded {
    static INT_PTR CALLBACK thunk(HWND handle, UINT msg, WPARAM wparam, LPARAM lparam) {
        T* window;
        if (msg == WM_INITDIALOG && (window = (T*)lparam)) {
            window->handle = handle;
            window->object(window);
        } else {
            window = Window(handle).object<T>();
        }
        if (window) return dispatch(window, msg, wparam, lparam);
        return FALSE;
    }
    static INT_PTR dispatch(T* window, UINT msg, WPARAM wparam, LPARAM lparam) {
        using namespace ui::native;
        switch (msg) {
        case WM_INITDIALOG: 
            window->open(); 
            return TRUE;
        case WM_DESTROY: 
            window->close();
            if (window == app->mainWindow()) ::PostQuitMessage(0);
            return TRUE;
        case WM_COMMAND:
            if (lparam) widget::Base::thunk_((HWND)lparam, HIWORD(wparam));
            return TRUE;
        case WM_DRAWITEM:
            widget::Base::drawThunk_((DRAWITEMSTRUCT*)lparam);
            return TRUE;
        }
        return FALSE;
    }
};

template <typename T>
struct TraitsBase : Dispatch <T, !T::UsesGraphics> {
    enum {
        classStyle = CS_DROPSHADOW * !!T::DropShadow,
        styleEx    = WS_EX_DLGMODALFRAME * !!T::SysCaption,
        style      = T::SysCaption ? WS_POPUP|WS_CAPTION|WS_SYSMENU : WS_POPUP
    };
    static const char* name() { return typeString<T>(); }
    static bool create(const Window* parent, T* window) { return createWindow<TraitsBase>(parent, window); }
};

template <typename T> struct Traits : TraitsBase <T> {};

template <typename T>
struct AppTraits : Traits <T> {
    enum { styleEx = 0, style = WS_POPUP|WS_SYSMENU|WS_MINIMIZEBOX }; // Simplified
    static bool create(const Window* parent, T* window) { return createWindow<AppTraits>(parent, window); }
};

template <typename T>
struct LayerTraits : Traits <T> {
    enum { classStyle = 0, styleEx = WS_EX_CONTROLPARENT, style = WS_CHILD };
    static bool create(const Window* parent, T* window) { return createWindow<LayerTraits>(parent, window); }
};

} // namespace details

template <typename T>
int app::run(bool ItCouldBeOnlyOne) {
    INITCOMMONCONTROLSEX icc = {sizeof(icc), ICC_WIN95_CLASSES};
    ::InitCommonControlsEx(&icc);
    
    using kali::app;
    app.alloc();
    app->details_ = app->autorelease(new AppDetails);
    app->details_->module_ = ::GetModuleHandle(0);
    T* window = new T;
    app->details_->mainWindow_ = window;

    if (!details::AppTraits<T>::create(nullptr, window)) return 0;
    app->autorelease(window);
    
    MSG msg;
    while (::GetMessage(&msg, NULL, 0, 0)) {
        if (!::IsDialogMessage(window->handle, &msg)) {
            ::TranslateMessage(&msg); ::DispatchMessage(&msg);
        }
    }
    app.release();
    return (int)msg.wParam;
}

template <typename T>
bool app::createWindow(const Window* parent, T* window) { return details::Traits<T>::create(parent, window); }

template <typename T>
bool app::createLayer(const Window* parent, T* window) { return details::LayerTraits<T>::create(parent, window); }

template <typename T>
bool app::loadLayer(const char* tag, const Window* parent, T* window) { 
    return details::loadWindow<details::DispatchLoaded<T>>(tag, parent, window); 
}

} // namespace kali

#endif
