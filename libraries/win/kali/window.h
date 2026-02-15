#ifndef KALI_WINDOW_INCLUDED
#define KALI_WINDOW_INCLUDED

#include "kali/platform.h"
#include "kali/string.h"

namespace kali {

// ============================================================================
// 1. DEFINICIONES DE GEOMETRÍA (OBJETOS COMPLETOS)
// ============================================================================
// Definimos estas estructuras aquí para evitar errores de "incomplete type"
// y asegurar compatibilidad con GCC 12.

struct Point {
    int x, y;
    Point(int x = 0, int y = 0) : x(x), y(y) {}
};

struct Size {
    int w, h;
    Size(int w = 0, int h = 0) : w(w), h(h) {}
};

struct Rect {
    int x, y, w, h;
    Rect(int x = 0, int y = 0, int w = 0, int h = 0) : x(x), y(y), w(w), h(h) {}
    
    int right() const  { return x + w; }
    int bottom() const { return y + h; }
    bool empty() const { return w <= 0 || h <= 0; }
    
    bool contains(const Point& p) const {
        return p.x >= x && p.x < (x + w) && p.y >= y && p.y < (y + h);
    }
};

// ============================================================================
// 2. ESTRUCTURA WINDOW
// ============================================================================

struct Window
{
    typedef HWND Handle;
    Handle handle;

    Window(Handle h = 0) : handle(h) {}
    
    // Conversión automática a HWND para APIs de Windows
    operator Handle () const { return handle; }

    // ........................................................................
    // GESTIÓN DE OBJETOS ASOCIADOS
    // ........................................................................

    template <typename T>
    T* object() const {
        return (T*) ::GetWindowLongPtr(handle, GWLP_USERDATA);
    }

    template <typename T>
    void object(T* ptr) {
        ::SetSetWindowLongPtr(handle, GWLP_USERDATA, (LONG_PTR) ptr);
    }

    // ........................................................................
    // UTILIDADES GRÁFICAS
    // ........................................................................

    void invalidate(bool erase = false) const {
        ::InvalidateRect(handle, 0, erase);
    }

    void update() const {
        ::UpdateWindow(handle);
    }

    // ........................................................................
    // ALERTAS Y MENSAJES (FIX DE AMBIGÜEDAD DE STRINGS)
    // ........................................................................

    bool alert(const char* text, const char* caption = "Message",
        const char* comments = 0) const
    {
        // [C4 FIX] Usamos el constructor variádico "%s" para eliminar la ambigüedad
        // entre el constructor de copia y el constructor de formato de kali::string.
        string s;
        if (comments) 
            s = string("%s    \n%s    ", text, comments);
        else 
            s = string("%s", text);

        return IDOK == ::MessageBox(handle, s(), caption,
            MB_OK | MB_ICONINFORMATION | MB_TASKMODAL | MB_SETFOREGROUND);
    }

    bool alertYesNo(const char* text, const char* caption = "Question",
        const char* comments = 0) const
    {
        string s;
        if (comments) 
            s = string("%s    \n%s    ", text, comments);
        else 
            s = string("%s", text);

        return IDYES == ::MessageBox(handle, s(), caption,
            MB_YESNO | MB_ICONQUESTION | MB_TASKMODAL | MB_SETFOREGROUND);
    }

    bool alertRetryCancel(const char* text, const char* caption = "Error",
        const char* comments = 0) const
    {
        string s;
        if (comments) 
            s = string("%s    \n%s    ", text, comments);
        else 
            s = string("%s", text);

        return IDRETRY == ::MessageBox(handle, s(), caption,
            MB_RETRYCANCEL | MB_ICONERROR | MB_TASKMODAL | MB_SETFOREGROUND);
    }

    // ........................................................................
    // MANEJO DE VENTANAS Y GEOMETRÍA
    // ........................................................................

    void show(int cmd = SW_SHOW) const {
        ::ShowWindow(handle, cmd);
    }

    void hide() const {
        ::ShowWindow(handle, SW_HIDE);
    }

    bool visible() const {
        return !!::IsWindowVisible(handle);
    }

    bool enabled() const {
        return !!::IsWindowEnabled(handle);
    }

    void enable(bool v = true) {
        ::EnableWindow(handle, v);
    }

    void disable() {
        enable(false);
    }

    void text(const char* s) {
        ::SetWindowText(handle, s);
    }

    string text() const {
        string s;
        ::GetWindowText(handle, s(), s.size);
        return s;
    }

    void focus() {
        ::SetFocus(handle);
    }

    void parent(Handle h) {
        ::SetParent(handle, h);
    }

    Handle parent() const {
        return ::GetParent(handle);
    }

    void position(int x, int y) {
        ::SetWindowPos(handle, 0, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOREDRAW);
    }

    // Devuelve un objeto Size completo (Resuelve error de tipo incompleto)
    Size size() const {
        RECT r;
        ::GetClientRect(handle, &r);
        return Size(r.right, r.bottom);
    }

    void size(int w, int h) {
        ::SetWindowPos(handle, 0, 0, 0, w, h, SWP_NOMOVE | SWP_NOZORDER | SWP_NOREDRAW);
    }
};

} // ~ namespace kali

#endif // ~ KALI_WINDOW_INCLUDED
