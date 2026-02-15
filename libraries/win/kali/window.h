#ifndef KALI_WINDOW_INCLUDED
#define KALI_WINDOW_INCLUDED

#include "kali/platform.h"
#include "kali/string.h"

namespace kali {

// ============================================================================
// 1. DEFINICIONES DE GEOMETRÍA BÁSICA (Sincronizadas con el Sistema UI)
// ============================================================================

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
// 2. ESTRUCTURA WINDOW (Motor de Gestión de Ventanas Win32)
// ============================================================================

struct Window
{
    typedef HWND Handle;
    Handle handle;

    Window(Handle h = 0) : handle(h) {}
    
    // Conversión implícita para APIs nativas de Windows
    operator Handle () const { return handle; }

    // ........................................................................
    // GESTIÓN DE OBJETOS ASOCIADOS (USERDATA)
    // ........................................................................

    template <typename T>
    T* object() const {
        return (T*) ::GetWindowLongPtr(handle, GWLP_USERDATA);
    }

    template <typename T>
    void object(T* ptr) {
        // [C4 FIX] Corregido typo SetSetWindowLongPtr -> SetWindowLongPtr
        ::SetWindowLongPtr(handle, GWLP_USERDATA, (LONG_PTR)ptr);
    }

    // ........................................................................
    // UTILIDADES GRÁFICAS Y ALERTAS
    // ........................................................................

    void invalidate(bool erase = false) const {
        ::InvalidateRect(handle, 0, erase);
    }

    void update() const {
        ::UpdateWindow(handle);
    }

    // Fix de Ambigüedad de Strings para GCC 12
    bool alert(const char* text, const char* caption = "Message",
        const char* comments = 0) const
    {
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

    // ........................................................................
    // MANEJO DE ESTADO Y PROPIEDADES
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

    void text(const char* s) {
        ::SetWindowText(handle, s);
    }

    string text() const {
        string s;
        ::GetWindowText(handle, s(), s.size);
        return s;
    }

    string title() const {
        return text();
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

    // ........................................................................
    // GEOMETRÍA DINÁMICA
    // ........................................................................

    Point position() const {
        RECT r;
        ::GetWindowRect(handle, &r);
        return Point(r.left, r.top);
    }

    void position(int x, int y) {
        ::SetWindowPos(handle, 0, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOREDRAW);
    }

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
