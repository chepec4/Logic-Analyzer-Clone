#ifndef KALI_APP_DETAILS_INCLUDED
#define KALI_APP_DETAILS_INCLUDED

#include <windows.h>
#include <commctrl.h>
#include "kali/window.h"
#include "kali/ui/native/widgets.h"
#include "kali/resources.h"

// [C4 FIX] Necesario para instanciar BufferedContext en Dispatch::thunk
#include "kali/graphics.h" 

namespace kali {

struct AppDetails : ReleaseAny {
    Module* module_;
    Window* mainWindow_;
    bool    graphics_;
    AppDetails() : module_(nullptr), mainWindow_(nullptr), graphics_(false) {}
};

inline Module* app::module()     const {return details_->module_;}
inline Window* app::mainWindow() const {return details_->mainWindow_;}
inline void    app::useThreads()       {}
inline void    app::initGraphics() {
#if defined KALI_GRAPHICS_INCLUDED
    if (!details_->graphics_) {
        // Inicializador gráfico si fuera necesario
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
    wcx.hCursor       = ::LoadCursor(nullptr, IDC_ARROW);
    wcx.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1); // Default
    return ::RegisterClassEx(&wcx) != 0;
}

template <typename Traits, typename T>
bool createWindow(const Window* parent, T* window) {
    if (!preinitWindow<Traits>()) {
        // Si falla porque ya existe, ignoramos. Si es otro error, fallamos.
        if (::GetLastError() != ERROR_CLASS_ALREADY_EXISTS) return false;
    }

    HWND hParent = parent ? parent->handle : nullptr;
    
    // Creación de la ventana nativa
    HWND hwnd = ::CreateWindowEx(
        Traits::styleEx,
        Traits::name(),
        nullptr, // No title init
        Traits::style,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        hParent,
        nullptr, // No menu
        app->module(),
        window // Pass 'this' to WM_NCCREATE
    );

    return (hwnd != nullptr);
}

// ----------------------------------------------------------------------------
// DISPATCHER DE MENSAJES (El corazón del loop de eventos)
// ----------------------------------------------------------------------------

template <typename T, bool Graphics = false>
struct Dispatch {
    static LRESULT CALLBACK dispatch(T* window, UINT msg, WPARAM w, LPARAM l) {
        return ::DefWindowProc(window->handle, msg, w, l);
    }
};

// Especialización para ventanas con gráficos (UsesGraphics = true)
template <typename T>
struct Dispatch <T, true> {
    static LRESULT CALLBACK dispatch(T* window, UINT msg, WPARAM w, LPARAM l) {
        switch (msg) {
            case WM_ERASEBKGND: 
                return 1; // Evitar parpadeo, nosotros pintamos
            
            case WM_PAINT: {
                PAINTSTRUCT ps;
                HDC dc = ::BeginPaint(window->handle, &ps);
                RECT r; ::GetClientRect(window->handle, &r);
                
                // [C4 FIX] BufferedContext ahora está completamente definido
                graphics::BufferedContext c(dc, r); 
                
                // Llamada al método draw() de sa::Display
                window->draw(c);
                
                ::EndPaint(window->handle, &ps);
                return 0;
            }
        }
        return ::DefWindowProc(window->handle, msg, w, l);
    }
};

// Thunk estático para conectar HWND -> T*
template <typename T, bool Graphics>
struct DispatchThunk : Dispatch<T, Graphics> {
    static LRESULT CALLBACK thunk(HWND hwnd, UINT msg, WPARAM w, LPARAM l) {
        T* window = nullptr;
        if (msg == WM_NCCREATE) {
            window = (T*)((LPCREATESTRUCT)l)->lpCreateParams;
            window->handle = hwnd;
            ::SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)window);
        } else {
            window = (T*)::GetWindowLongPtr(hwnd, GWLP_USERDATA);
        }

        if (window) {
            // Hook opcional del usuario
            LRESULT res = 0;
            if (window->msgHook(res, msg, w, l)) return res;
            
            return Dispatch<T, Graphics>::dispatch(window, msg, w, l);
        }
        return ::DefWindowProc(hwnd, msg, w, l);
    }
};

// Traits para Ventanas
template <typename T>
struct TraitsBase : DispatchThunk <T, T::UsesGraphics> {
    enum {
        classStyle = CS_DBLCLKS | (T::DropShadow ? CS_DROPSHADOW : 0) | CS_HREDRAW | CS_VREDRAW,
        styleEx    = T::SysCaption ? WS_EX_DLGMODALFRAME : 0,
        style      = T::SysCaption ? (WS_POPUP | WS_CAPTION | WS_SYSMENU) : (WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS)
    };
};

template <typename T>
struct Traits : TraitsBase <T> {
    static const char* name() { return "KaliWindow"; }
    static bool create(const Window* parent, T* window) { 
        return createWindow<Traits>(parent, window); 
    }
};

// Traits para Layers (Controles hijos)
template <typename T>
struct LayerTraits : Traits <T> {
    // Heredamos thunk pero cambiamos estilos
    enum { 
        classStyle = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW,
        styleEx    = WS_EX_CONTROLPARENT, 
        style      = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS
    };
    static const char* name() { return "KaliLayer"; }
    static bool create(const Window* parent, T* window) { 
        return createWindow<LayerTraits>(parent, window); 
    }
};

} // namespace details

// Implementación de métodos de app
template <typename T>
bool app::createWindow(const Window* parent, T* window) { 
    return details::Traits<T>::create(parent, window); 
}

template <typename T>
bool app::createLayer(const Window* parent, T* window) { 
    return details::LayerTraits<T>::create(parent, window); 
}

template <typename T>
bool app::loadLayer(const char* tag, const Window* parent, T* window) {
    // Stub para carga de recursos si fuera necesario
    return createLayer(parent, window);
}

} // namespace kali

#endif
