#ifndef KALI_WINDOW_INCLUDED
#define KALI_WINDOW_INCLUDED

#include "kali/string.h"
#include <windows.h>

namespace kali {

struct Window
{
    HWND handle;

    Window(HWND h = 0) : handle(h) {}
    operator HWND() const { return handle; }

    bool isValid() const { return handle != 0 && ::IsWindow(handle); }

    void text(const char* s) { ::SetWindowText(handle, s); }
    
    void position(int x, int y) 
    { 
        ::SetWindowPos(handle, 0, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER); 
    }

    void size(int w, int h) 
    { 
        ::SetWindowPos(handle, 0, 0, 0, w, h, SWP_NOMOVE | SWP_NOZORDER); 
    }

    Rect rect() const
    {
        RECT r;
        ::GetWindowRect(handle, &r);
        return Rect(r.left, r.top, r.right - r.left, r.bottom - r.top);
    }

    // =========================================================================
    // REPARACIÓN METICULOSA: OPERADOR TERNARIO Y TIPOS DE STRING
    // =========================================================================

    bool alert(const char* title, const char* text, const char* comments = 0) const
    {
        // Forzamos que ambos resultados del operador sean del mismo tipo (string)
        // para evitar el error: 'operands to ?: have different types'
        const string s = !comments ? string(text) 
                                   : string("%s    \n%s    ", text, comments);
        return ::MessageBox(handle, s.c_str(), title, MB_TASKMODAL | MB_ICONWARNING | MB_OK) == IDOK;
    }

    bool alertYesNo(const char* title, const char* text, const char* comments = 0) const
    {
        const string s = !comments ? string(text) 
                                   : string("%s    \n%s    ", text, comments);
        return ::MessageBox(handle, s.c_str(), title, MB_TASKMODAL | MB_ICONQUESTION | MB_YESNO) == IDYES;
    }

    bool alertRetryCancel(const char* title, const char* text, const char* comments = 0) const
    {
        const string s = !comments ? string(text) 
                                   : string("%s    \n%s    ", text, comments);
        return ::MessageBox(handle, s.c_str(), title, MB_TASKMODAL | MB_ICONERROR | MB_RETRYCANCEL) == IDRETRY;
    }
    
    // =========================================================================

    void invalidate(const Rect* r = 0, bool erase = true)
    {
        if (r)
        {
            RECT wr = { r->x, r->y, r->x + r->w, r->y + r->h };
            ::InvalidateRect(handle, &wr, erase);
        }
        else
        {
            ::InvalidateRect(handle, 0, erase);
        }
    }

    void update() { ::UpdateWindow(handle); }
    void show(int cmd = SW_SHOW) { ::ShowWindow(handle, cmd); }
    void hide() { ::ShowWindow(handle, SW_HIDE); }
    void focus() { ::SetFocus(handle); }
};

} // ~ namespace kali

#endif // ~ KALI_WINDOW_INCLUDED

