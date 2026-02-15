#ifndef KALI_UI_NATIVE_WIDGETS_INCLUDED
#define KALI_UI_NATIVE_WIDGETS_INCLUDED

// VALIDACIÓN: Pre-declaramos el namespace para evitar el error "huevo o gallina"
namespace kali { namespace ui { namespace native { namespace widget { struct Interface; } } } }

#include "kali/ui/native.h"
#include "kali/ui/native/widgets.base.h"

namespace kali {
namespace ui {
namespace native {
namespace widget {

// ............................................................................

struct ResourceCtor
{
    struct Aux
    {
        Window::Handle handle;

        Aux(Window::Handle h) : handle(h) {}

        // VALIDACIÓN: Estos operadores DEBEN ser públicos.
        // Si fueran privados, el Editor no podría crear los botones y combos.
    public:
        template <typename T> operator T () const { return (T)handle; }
        template <typename T> operator T* () const { return (T*)handle; }
    };

    Aux operator () (int id) const
    {
        return Aux(::GetDlgItem(parent->handle, id));
    }

    const Window* parent;
    ResourceCtor(const Window* p) : parent(p) {}
};

// ............................................................................
// Definiciones completas de los Widgets
// Se asegura la compatibilidad con los mensajes de Windows (SendMessage)
// ............................................................................

struct Combo : Interface {
    static const uint32_t style_ = CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP;
    static const char* class_() { return "COMBOBOX"; }
    
    void add(const char* text) { SendMessage(handle, CB_ADDSTRING, 0, (LPARAM)text); }
    void clear() { SendMessage(handle, CB_RESETCONTENT, 0, 0); }
    int  value() const { return SendMessage(handle, CB_GETCURSEL, 0, 0); }
    void value(int v) { SendMessage(handle, CB_SETCURSEL, v, 0); }
    
    void ctor(const Window* parent, const Rect& r) { Interface::create(parent, r, style_, 0, class_()); }
    void ctor(const Window* parent, int id) { handle = GetDlgItem(*parent, id); }
};

struct Toggle : Interface {
    static const uint32_t style_ = BS_AUTOCHECKBOX | WS_TABSTOP;
    static const char* class_() { return "BUTTON"; }
    
    int  value() const { return SendMessage(handle, BM_GETCHECK, 0, 0); }
    void value(int v) { SendMessage(handle, BM_SETCHECK, v, 0); }
    
    void ctor(const Window* parent, const Rect& r, const char* text = 0) { 
        Interface::create(parent, r, style_, 0, class_(), text); 
    }
    void ctor(const Window* parent, int id) { handle = GetDlgItem(*parent, id); }
};

struct Button : Interface {
    static const uint32_t style_ = BS_PUSHBUTTON | WS_TABSTOP;
    static const char* class_() { return "BUTTON"; }
    
    void ctor(const Window* parent, const Rect& r, const char* text = 0) { 
        Interface::create(parent, r, style_, 0, class_(), text); 
    }
};

// Stubs para completar los tipos requeridos por native.h y evitar errores de "tipo incompleto"
struct Edit : Interface { static const char* class_() { return "EDIT"; } };
struct Text : Interface { static const char* class_() { return "STATIC"; } };
struct TextRight : Interface { static const char* class_() { return "STATIC"; } };
struct LayerTabs : Interface { static const char* class_() { return "SysTabControl32"; } }; 

// ............................................................................

} // ~ namespace widget
} // ~ namespace native
} // ~ namespace ui
} // ~ namespace kali

#endif // ~ KALI_UI_NATIVE_WIDGETS_INCLUDED
