#ifndef SA_RESIZER_INCLUDED
#define SA_RESIZER_INCLUDED

#include "kali/ui/native.h"

namespace sa {

using namespace kali;

// ============================================================================
// RESIZER (Gestor de Redimensionado en Tiempo Real)
// ============================================================================

struct Resizer : ui::native::LayerBase
{
    // Lógica de polling llamada desde el timer de sa::Display
    int poll(HWND editor_)
    {
        if (!editor)
            ctor(editor_);

        // Si la ventana padre (VST Host) se oculta, nos ocultamos
        if (!parent || !::IsWindowVisible(parent))
            return ::ShowWindow(handle, SW_HIDE);

        if (!resizingNow) {
            Rect r = getWindowRect(parent);
            if (curRect != r)
            {
                curRect  = r;
                // Sincronizar posición con el Host sin activar la ventana
                return ::SetWindowPos(handle, 0, r.x, r.y, r.w, r.h,
                    SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOCOPYBITS | SWP_SHOWWINDOW);
            }
        }

        // SW_SHOWNA: Mostrar sin activar (evita robar foco al teclado del Host)
        return ::IsWindowVisible(handle) ? ~0 : ::ShowWindow(handle, SW_SHOWNA);
    }

    // Hook para interceptar mensajes de redimensionado
    bool msgHook(LRESULT& result, UINT msg, WPARAM wparam, LPARAM lparam)
    {
        switch (msg)
        {
        case WM_ERASEBKGND:
            result = 1; return true; // Transparencia visual

        case WM_NCHITTEST:
            result = HTTRANSPARENT; return true; // Click-through al editor

        case WM_ENTERSIZEMOVE: resizingNow = true;  break;
        case WM_EXITSIZEMOVE:  resizingNow = false; break;
        }
        return false;
    }

private:
    void ctor(HWND editor_)
    {
        editor = editor_;
        parent = ::GetParent(editor);
        if (parent) {
            // [C4 FIX] x64 Compliance: GetWindowLongPtr
            LONG_PTR exStyle = ::GetWindowLongPtr(handle, GWL_EXSTYLE);
            // Hacer la ventana transparente a los eventos de ratón pero visible (Layered)
            ::SetWindowLongPtr(handle, GWL_EXSTYLE, (exStyle & ~WS_EX_TRANSPARENT) | WS_EX_LAYERED);
            ::SetLayeredWindowAttributes(handle, 0, 1, LWA_ALPHA); // Alpha casi 0
        }
    }

    static Rect getWindowRect(HWND w) {
        RECT r; ::GetWindowRect(w, &r);
        return Rect(r.left, r.top, r.right - r.left, r.bottom - r.top);
    }

public:
    Resizer(AudioEffectX* plugin) :
        plugin(plugin),
        editor(nullptr),
        parent(nullptr),
        resizingNow(false)
    {}

    // Limpieza segura
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

// ============================================================================
// TRAITS DEL SISTEMA DE VENTANAS (Template Specialization)
// ============================================================================

namespace kali    {
namespace details {

// Traits específicos para el Resizer (Ventana PopUp Transparente)
template <>
struct Traits <sa::Resizer> : TraitsBase <sa::Resizer>
{
    enum
    {
        styleEx = WS_EX_TRANSPARENT, // Click-through inicial
        style   = WS_POPUP,          // Sin bordes ni decoración
    };
};

} // ~ namespace details
} // ~ namespace kali

#endif
