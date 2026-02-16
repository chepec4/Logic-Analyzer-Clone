#ifndef KALI_UI_NATIVE_DETAILS_INCLUDED
#define KALI_UI_NATIVE_DETAILS_INCLUDED

#include <windows.h>
#include <commctrl.h>
#include "kali/dbgutils.h"

namespace kali    {
namespace ui      {
namespace native  {
namespace details {

// Clonado de ventana (útil para controles complejos heredados)
inline Window::Handle cloneWindow(Window::Handle window, int styleRemove = 0, int styleAdd = 0, bool destroy = false)
{
    string class_, text;
    ::GetClassNameA(window, class_(), class_.size);
    ::GetWindowTextA(window, text(), text.size);
    
    // [C4 FIX] x64 Compliance: GetWindowLongPtr en lugar de GetWindowLong
    LONG_PTR style = ::GetWindowLongPtr(window, GWL_STYLE);
    style = (style & ~styleRemove) | styleAdd;
    LONG_PTR styleEx = ::GetWindowLongPtr(window, GWL_EXSTYLE);

    RECT r;
    Window::Handle parent = ::GetParent(window);
    ::GetWindowRect(window, &r);
    // MapWindowPoints con nullptr convierte de pantalla a cliente del padre
    ::MapWindowPoints(nullptr, parent, (POINT*) &r, 2);

    // Workaround para ComboBox: obtener el tamaño real incluyendo el drop list
    if (style & CBS_DROPDOWNLIST)
    {
        RECT rr;
        ::SendMessage(window, CB_GETDROPPEDCONTROLRECT, 0, (LPARAM) &rr);
        r.bottom = r.top - rr.top + rr.bottom;
        style |= ~styleRemove & WS_VSCROLL;
    }

    Window::Handle handle = ::CreateWindowExA(
        styleEx, class_, text, style,
        r.left, r.top, r.right - r.left, r.bottom - r.top,
        parent, nullptr, ::GetModuleHandle(nullptr), nullptr
    );

    // Clonar la fuente original
    if (handle) {
        HFONT font = (HFONT)::SendMessage(window, WM_GETFONT, 0, 0);
        ::SendMessage(handle, WM_SETFONT, (WPARAM)font, 0);
    }

    if (destroy) ::DestroyWindow(window);

    return handle;
}

// Wrapper seguro para carga dinámica de APIs (DLLs del sistema)
template <typename F>
struct DynamicApi
{
    operator F*()   const {return func;}
    operator bool() const {return !!func;}

    DynamicApi(const char* lib, const char* f) {ctor(lib, f);}
    ~DynamicApi() { if (module) ::FreeLibrary(module); }

private:
    HMODULE module;
    F* func;

    DynamicApi(const DynamicApi&);
    DynamicApi& operator = (const DynamicApi&);

    void ctor(const char* lib, const char* f)
    {
        func   = nullptr;
        module = ::LoadLibraryA(lib);
        if (!module) {
            // trace("%s: LoadLibrary(%s) failed [%i]\n", FUNCTION_, lib, ::GetLastError());
            return;
        }

        #pragma warning(push)
        #pragma warning(disable: 4191) // Ignorar warning de cast de puntero de función
        func = (F*) ::GetProcAddress(module, f);
        #pragma warning(pop)
        
        if (!func) {
            // trace("%s: GetProcAddress(%s) failed [%i]\n", FUNCTION_, f, ::GetLastError());
        }
    }
};

} // ~ namespace details
} // ~ namespace native
} // ~ namespace ui
} // ~ namespace kali

#endif
