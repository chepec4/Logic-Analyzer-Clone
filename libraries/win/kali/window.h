#ifndef KALI_WINDOW_INCLUDED
#define KALI_WINDOW_INCLUDED

#include "kali/string.h"
// CRÍTICO: Usamos geometry.h para tener Rect y Size sin conflictos
#include "kali/geometry.h" 
#include <windows.h>

namespace kali {

struct Window
{
    // [FIX 1] Definimos Handle para que widgets.h no falle
    typedef HWND Handle;

    HWND handle;

    Window(HWND h = 0) : handle(h) {}
    operator HWND() const { return handle; }

    bool isValid() const { return handle != 0 && ::IsWindow(handle); }

    void text(const char* s) { ::SetWindowText(handle, s); }
    
    // [FIX 2] Métodos de posicionamiento y tamaño requeridos por sa.resizer.h
    void position(int x, int y) { ::SetWindowPos(handle, 0, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER); }
    void size(int w, int h)     { ::SetWindowPos(handle, 0, 0, 0, w, h, SWP_NOMOVE | SWP_NOZORDER); }
    
    // Sobrecarga para aceptar 'Size' (Soluciona error en sa.display.h)
    void size(const Size& s)    { size(s.w, s.h); }

    Rect rect() const
    {
        RECT r;
        ::GetWindowRect(handle, &r);
        return Rect(r.left, r.top, r.right - r.left, r.bottom - r.top);
    }

    // [FIX 3] Getters que faltaban
    Point position() const { Rect r = rect(); return Point(r.x, r.y); }
    Size size() const      { Rect r = rect(); return Size(r.w, r.h); }

    // [FIX 4] Método estático para el tamaño de pantalla
    static Size screenSize() 
    { 
        return Size(::GetSystemMetrics(SM_CXSCREEN), ::GetSystemMetrics(SM_CYSCREEN)); 
    }

    // [FIX 5] Método destroy faltante
    void destroy() 
    { 
        if (isValid()) ::DestroyWindow(handle); 
    }

    // --- FIX DE ALERTAS ---
    bool alert(const char* title, const char* text, const char* comments = 0) const
    {
        const string s = !comments ? string("%s", text) 
                                   : string("%s    \n%s    ", text, comments);
        return ::MessageBox(handle, (const char*)s, title, MB_TASKMODAL | MB_ICONWARNING | MB_OK) == IDOK;
    }

    bool alertYesNo(const char* title, const char* text, const char* comments = 0) const
    {
        const string s = !comments ? string("%s", text) 
                                   : string("%s    \n%s    ", text, comments);
        return ::MessageBox(handle, (const char*)s, title, MB_TASKMODAL | MB_ICONQUESTION | MB_YESNO) == IDYES;
    }

    bool alertRetryCancel(const char* title, const char* text, const char* comments = 0) const
    {
        const string s = !comments ? string("%s", text) 
                                   : string("%s    \n%s    ", text, comments);
        return ::MessageBox(handle, (const char*)s, title, MB_TASKMODAL | MB_ICONERROR | MB_RETRYCANCEL) == IDRETRY;
    }

    void invalidate(const Rect* r = 0, bool erase = true)
    {
        if (r) {
            RECT wr = { r->x, r->y, r->x + r->w, r->y + r->h };
            ::InvalidateRect(handle, &wr, erase);
        } else {
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
