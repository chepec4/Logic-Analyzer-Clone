#ifndef KALI_WINDOW_INCLUDED
#define KALI_WINDOW_INCLUDED

#include "kali/platform.h"
#include "kali/string.h"

namespace kali {

// Forward declarations para asegurar que el compilador reconozca los tipos de geometría
struct Point;
struct Size;
template <typename T> struct Rect;

// ............................................................................

struct Window
{
    typedef HWND Handle;

    Handle handle;

    Window(Handle h = 0) : handle(h) {}
    
    // Conversión automática a HWND para APIs de Windows
    operator Handle () const { return handle; }

    // ........................................................................
    // GESTIÓN DE OBJETOS ASOCIADOS (USERDATA)
    // ........................................................................

    template <typename T>
    T* object() const
    {
        return (T*) ::GetWindowLongPtr(handle, GWLP_USERDATA);
    }

    template <typename T>
    void object(T* ptr)
    {
        ::SetWindowLongPtr(handle, GWLP_USERDATA, (LONG_PTR) ptr);
    }

    // ........................................................................
    // UTILIDADES GRÁFICAS
    // ........................................................................

    void invalidate(bool erase = false) const
    {
        ::InvalidateRect(handle, 0, erase);
    }

    void update() const
    {
        ::UpdateWindow(handle);
    }

    // ........................................................................
    // ALERTAS Y MENSAJES (CORREGIDO PARA EVITAR AMBIGÜEDAD)
    // ........................................................................

    bool alert(const char* text, const char* caption = "Message",
        const char* comments = 0) const
    {
        // [C4 FIX] Usamos asignación directa para evitar la ambigüedad del constructor
        string s;
        if (comments) 
            s.format("%s    \n%s    ", text, comments);
        else 
            s = text; // Operación no ambigua

        return IDOK == ::MessageBox(handle, s(), caption,
            MB_OK | MB_ICONINFORMATION | MB_TASKMODAL | MB_SETFOREGROUND);
    }

    bool alertYesNo(const char* text, const char* caption = "Question",
        const char* comments = 0) const
    {
        string s;
        if (comments) 
            s.format("%s    \n%s    ", text, comments);
        else 
            s = text;

        return IDYES == ::MessageBox(handle, s(), caption,
            MB_YESNO | MB_ICONQUESTION | MB_TASKMODAL | MB_SETFOREGROUND);
    }

    bool alertRetryCancel(const char* text, const char* caption = "Error",
        const char* comments = 0) const
    {
        string s;
        if (comments) 
            s.format("%s    \n%s    ", text, comments);
        else 
            s = text;

        return IDRETRY == ::MessageBox(handle, s(), caption,
            MB_RETRYCANCEL | MB_ICONERROR | MB_TASKMODAL | MB_SETFOREGROUND);
    }

    // ........................................................................
    // MANEJO DE VENTANAS Y GEOMETRÍA
    // ........................................................................

    void show(int cmd = SW_SHOW) const
    {
        ::ShowWindow(handle, cmd);
    }

    void hide() const
    {
        ::ShowWindow(handle, SW_HIDE);
    }

    bool visible() const
    {
        return !!::IsWindowVisible(handle);
    }

    bool enabled() const
    {
        return !!::IsWindowEnabled(handle);
    }

    void enable(bool v = true)
    {
        ::EnableWindow(handle, v);
    }

    void disable()
    {
        enable(false);
    }

    void text(const char* s)
    {
        ::SetWindowText(handle, s);
    }

    string text() const
    {
        string s;
        ::GetWindowText(handle, s(), s.size);
        return s;
    }

    void focus()
    {
        ::SetFocus(handle);
    }

    void parent(Handle h)
    {
        ::SetParent(handle, h);
    }

    Handle parent() const
    {
        return ::GetParent(handle);
    }

    // Métodos de posición/tamaño optimizados para llamadas internas de Kali
    void position(int x, int y)
    {
        ::SetWindowPos(handle, 0, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOREDRAW);
    }

    void size(int w, int h)
    {
        ::SetWindowPos(handle, 0, 0, 0, w, h, SWP_NOMOVE | SWP_NOZORDER | SWP_NOREDRAW);
    }
};

// ............................................................................

} // ~ namespace kali

#endif // ~ KALI_WINDOW_INCLUDED
