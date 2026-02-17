#ifndef KALI_WIN_WIDGETS_INCLUDED
#define KALI_WIN_WIDGETS_INCLUDED

#include <windows.h>
#include <commctrl.h>

namespace kali {
namespace ui {
namespace native {
namespace widget {

// [C4 MASTER SYNC] Implementación de la Base de Widgets
struct Base : Interface {
    HWND handle;
    Base() : handle(nullptr) {}

    // Implementación de la interfaz obligatoria
    Window::Handle expose() const override { return handle; }
    bool enable() const override { return handle && IsWindowEnabled(handle); }
    void enable(bool v) override { if(handle) EnableWindow(handle, v); }
    bool visible() const override { return handle && IsWindowVisible(handle); }
    void visible(bool v) override { if(handle) ShowWindow(handle, v ? SW_SHOW : SW_HIDE); }
    int  value() const override { return 0; }
    void value(int) override {}
    int  range() const override { return 0; }
    void range(int) override {}
    string text() const override { return string(); }
    void text(const char*) override {}

    void ctor(const Window* p, HWND h) { 
        handle = h; 
        Window(h).object(this); 
    }
};

// Definición de widgets concretos (Stubs para enlace)
struct Text    : Base { };
struct Edit    : Base { };
struct Button  : Base { };
struct Toggle  : Button { };
struct Combo   : Base { };
struct Toolbar : Base { };
struct Fader   : Base { };
struct LayerTabs : Base { 
    void add(const char* tag, LayerBase* layer) { /* Lógica de pestañas */ }
};

// [C4 FIX] Factory Method global dentro del namespace
template <typename T>
inline T* Ctor(const Window* parent, const Rect& r, const char* text = "") {
    // Aquí iría la lógica de CreateWindowExA para cada control
    return new T(); 
}

} // namespace widget
} // namespace native
} // namespace ui
} // namespace kali

#endif
