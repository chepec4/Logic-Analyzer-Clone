#ifndef KALI_UI_NATIVE_WIDGETS_H_INCLUDED
#define KALI_UI_NATIVE_WIDGETS_H_INCLUDED

#include <windows.h>
#include <commctrl.h>

namespace kali {
namespace ui {
namespace native {

// [C4 MASTER FIX] Estructura Font para Windows
struct Font : ReleaseAny {
    struct Scale {
        int x_, y_;
        Scale(int x, int y) : x_(x), y_(y) {}
        int x(int v) const { return (v * x_ + 3) / 6; }
        int y(int v) const { return (v * y_) / 13; }
    };

    static const Font& main() {
        static Font* inst = nullptr;
        if (!inst) inst = app->autorelease(new Font);
        return *inst;
    }

    HFONT hFont;
    Font() {
        NONCLIENTMETRICS ncm = { sizeof(ncm) };
        SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(ncm), &ncm, 0);
        hFont = CreateFontIndirect(&ncm.lfMessageFont);
    }
    ~Font() { DeleteObject(hFont); }
    Scale scale() const { return Scale(8, 16); }
};

namespace widget {

struct Base : Interface {
    HWND handle;
    Base() : handle(nullptr) {}
    Window::Handle expose() const override { return handle; }
    void ctor(const Window* p, HWND h) { 
        handle = h; 
        Window(h).object(this); 
    }
    
    bool enable() const override { return IsWindowEnabled(handle); }
    void enable(bool v) override { EnableWindow(handle, v); }
    bool visible() const override { return IsWindowVisible(handle); }
    void visible(bool v) override { ShowWindow(handle, v ? SW_SHOW : SW_HIDE); }
    int  value() const override { return 0; }
    void value(int) override {}
    int  range() const override { return 0; }
    void range(int) override {}
    string text() const override { return string(); }
    void text(const char*) override {}
};

// Implementación de widgets concretos
struct Button    : Base { static const char* class_() { return "BUTTON"; } };
struct Combo     : Base { static const char* class_() { return "COMBOBOX"; } };
struct Text      : Base { static const char* class_() { return "STATIC"; } };
struct Edit      : Base { static const char* class_() { return "EDIT"; } };
struct Toggle    : Button { };
struct Toolbar   : Base { };
struct ColorWell : Base { };
struct Stepper   : Base { };
struct Fader     : Base { };

} // namespace widget
} // namespace native
} // namespace ui
} // namespace kali

#endif
