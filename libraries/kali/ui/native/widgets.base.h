#ifndef KALI_UI_NATIVE_WIDGETS_BASE_INCLUDED
#define KALI_UI_NATIVE_WIDGETS_BASE_INCLUDED

#include "kali/function.h"
#include "kali/containers.h"
#include "kali/ui/base.h"

// ............................................................................

namespace kali   {
namespace ui     {
namespace native {
namespace widget {

// ............................................................................

struct Parent
{
    // Nota: Window* depende de la definición forward o include de la plataforma.
    // Esto se resolverá al compilar con libraries/win/kali/window.h
    virtual Window* window()         = 0;
    virtual void attach(ReleaseAny*) = 0;
    virtual ~Parent() {}
};

struct Interface : ui::widget::Interface, UsesCallback
{
    virtual int  range() const = 0;
    virtual void range(int v)  = 0;

    virtual string text() const    = 0;
    virtual void text(const char*) = 0;

    // Acceso al handle nativo (HWND en Windows, NSView en Mac)
    virtual Window::Handle expose() const = 0;

    Callback extCallback;
};

// ............................................................................

} // ~ namespace widget
} // ~ namespace native
} // ~ namespace ui
} // ~ namespace kali

#endif // ~ KALI_UI_NATIVE_WIDGETS_BASE_INCLUDED
