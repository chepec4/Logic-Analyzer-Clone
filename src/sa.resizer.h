#ifndef SA_RESIZER_INCLUDED
#define SA_RESIZER_INCLUDED

#include "kali/ui/native.h"

namespace sa {

using namespace kali;

// ============================================================================
// RESIZER: GESTOR DE REDIMENSIONADO DE VENTANA
// ============================================================================

struct Resizer : ui::native::LayerBase
{
    // Lógica de polling llamada desde sa::Display
    int poll(HWND editor_)
    {
        if (!editor)
            ctor(editor_);

        if (!parent || !::IsWindowVisible(parent))
            return ::ShowWindow(handle, SW_HIDE);

        if (!resizingNow) {
            Rect r = getWindowRect(parent);
            if (curRect != r)
            {
                curRect  = r;
                return ::SetWindowPos(handle, 0, r.x, r.y, r.w, r.h,
                    SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOCOPYBITS | SWP_SHOWWINDOW);
            }
        }

        return ::IsWindowVisible(handle) ? ~0 : ::ShowWindow(handle, SW_SHOWNA);
    }

    // Hook de mensajes de Windows
    bool msgHook(LRESULT& result, UINT msg, WPARAM wparam, LPARAM lparam)
    {
        switch (msg)
        {
        case WM_ERASEBKGND:
            result = ~0;
            break;

        case WM_ENTERSIZEMOVE:
            resizingNow = true;
            break;

        case WM_EXITSIZEMOVE:
            // trace.full("msgHook: WM_EXITSIZEMOVE\n");
            resizingNow = false;
            ::SetForegroundWindow(parent);
            break;

        case WM_NCHITTEST:
            result = ::DefWindowProc(handle, msg, wparam, lparam);
            // Solo permitimos redimensionar desde la esquina inferior derecha
            switch (result)
            {
            case HTRIGHT:
            case HTBOTTOM:
            case HTBOTTOMRIGHT:
                break;
            default:
                result = HTTRANSPARENT;
            }
            break;
        }

        return true;
    }

    // Callback de redimensionado finalizado
    void resized()
    {
        if (resizingNow)
            resizeEditor(getWindowRect(handle));
    }

private:

    // [C4 FIX] Helper estático renombrado para evitar colisión con kali::Rect
    static Rect getWindowRect(HWND h)
    {
        RECT r;
        ::GetWindowRect(h, &r);
        return Rect(r.left, r.top, r.right - r.left, r.bottom - r.top);
    }

    void resizeEditor(const Rect& newRect)
    {
        int w = newRect.w - curRect.w;
        int h = newRect.h - curRect.h;
        curRect = newRect;
        
        // Uso de Window Wrapper moderno
        Size s = Window(editor).size();
        Window(editor).size(s.w += w, s.h += h);

        if (plugin->canHostDo("sizeWindow") > 0) {
            if (plugin->sizeWindow(s.w, s.h))
                return; 
        }

        // Fallback manual si el host no soporta sizeWindow
        HWND ancestor = ::GetAncestor(editor, GA_PARENT);
        Rect r = getWindowRect(ancestor);
        
        ::SetWindowPos(ancestor, 0, 0, 0, r.w + w, r.h + h, SWP_NOMOVE | SWP_NOZORDER);

        r = newRect;
        ::SetWindowPos(parent, 0, r.x, r.y, r.w, r.h, SWP_NOMOVE | SWP_NOZORDER);
    }

    void ctor(HWND editor_)
    {
        editor = editor_;
        parent = editor_;
        
        // Búsqueda heurística del padre correcto en la jerarquía del VST
        char buffer[256];
        string name;
        do
        {
            parent = ::GetAncestor(parent, GA_PARENT);
            ::GetWindowTextA(parent, buffer, 256);
            name = string("%s", buffer);
        }
        while (!strstr(name, NAME) && (::GetWindowLongPtr(parent, GWL_STYLE) & WS_CHILD));

        Window p(parent);
        
        // [C4 FIX] Creación segura vía App Singleton
        if (kali::app) kali::app->createWindow(&p, this);
        
        ::ShowWindow(handle, SW_HIDE);

        // [C4 FIX] Uso de SetWindowLongPtr para compatibilidad x64
        if (LOBYTE(::GetVersion()) > 5) {
            LONG_PTR exStyle = ::GetWindowLongPtr(handle, GWL_EXSTYLE);
            ::SetWindowLongPtr(handle, GWL_EXSTYLE, (exStyle & ~WS_EX_TRANSPARENT) | WS_EX_LAYERED);
            ::SetLayeredWindowAttributes(handle, 0, 1, LWA_ALPHA);
        }
    }

public:
    Resizer(AudioEffectX* plugin) :
        plugin(plugin),
        editor(nullptr),
        parent(nullptr),
        resizingNow(false)
    {}

    // [C4 FIX] Eliminación de 'tf' y uso de Lifecycle moderno
    void close() override {
        if (handle) {
            ::DestroyWindow(handle);
            handle = nullptr;
        }
    }
    
    ~Resizer() { close(); }

private:
    AudioEffectX* plugin;
    HWND          editor;
    HWND          parent;
    Rect          curRect;
    bool          resizingNow;
};

} // ~ namespace sa

// ............................................................................
// TRAITS DEL SISTEMA DE VENTANAS (Template Specialization)
// ............................................................................

namespace kali    {
namespace details {

template <>
struct Traits <sa::Resizer> : TraitsBase <sa::Resizer>
{
    enum
    {
        styleEx = WS_EX_TRANSPARENT,
        style   = WS_POPUP | WS_SIZEBOX
    };

    static bool create(const Window* parent, sa::Resizer* window)
    {
        return createWindow<Traits>(parent, window);
    }
};

} // ~ namespace details
} // ~ namespace kali

#endif // ~ SA_RESIZER_INCLUDED
