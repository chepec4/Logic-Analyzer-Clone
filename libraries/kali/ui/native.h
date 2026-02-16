#ifndef KALI_UI_NATIVE_INCLUDED
#define KALI_UI_NATIVE_INCLUDED

#include "kali/ui/native/widgets.base.h"

namespace kali {
namespace ui {
namespace native {

// [C4 MASTER FIX] Declaraciones adelantadas para romper la dependencia circular con widgets.h
namespace widget {
    struct Interface;
    struct Button;
    struct Combo;
    struct Text;
    struct Edit;
    struct Toggle;
    struct Toolbar;
    struct ColorWell;
    struct Stepper;
    struct Fader;
}

// 1. TYPEDEFS (Punteros Inteligentes)
typedef Ptr<widget::Interface> AnyWidget;
typedef Ptr<widget::Button>    Button;
typedef Ptr<widget::Combo>     Combo;
typedef Ptr<widget::Text>      Text;
typedef Ptr<widget::Text>      TextRight;
typedef Ptr<widget::Text>      Label;
typedef Ptr<widget::Edit>      Edit;
typedef Ptr<widget::Toggle>    Toggle;
typedef Ptr<widget::Toolbar>   Toolbar;
typedef Ptr<widget::ColorWell> ColorWell;
typedef Ptr<widget::Stepper>   Stepper;
typedef Ptr<widget::Fader>     Fader;

// 2. WINDOW BASE
struct WindowBase : Window {
    enum { UsesGraphics = 1, DropShadow = 0, SysCaption = 0 };
    virtual ~WindowBase() {}

    void centerAt(const Window* at = nullptr) {
        Rect r = at ? Rect(at->position(), at->size()) : Rect(0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
        Size s = size();
        this->position(r.x + (r.w - s.w) / 2, r.y + (r.h - s.h) / 2);
    }
    
    // Firma virtual requerida para el dispatcher
    virtual void draw(graphics::BufferedContext&) {}
};

struct LayerBase : WindowBase {
    virtual bool open() { return true; }
    virtual void close() { Window::handle = nullptr; }
};

} // namespace native
} // namespace ui
} // namespace kali

// Incluimos la implementación de plataforma al final para que vea los typedefs
#include "kali/ui/native/widgets.h"

#endif
