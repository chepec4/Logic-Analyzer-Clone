#ifndef KALI_UI_NATIVE_WIDGETS_INCLUDED
#define KALI_UI_NATIVE_WIDGETS_INCLUDED

// [FIX 1] PRE-DECLARACIÓN COMPLETA
// Esto evita que native.h falle al no encontrar estos tipos.
namespace kali { namespace ui { namespace native { namespace widget { 
    struct Interface; 
    struct Combo;
    struct Toggle;
    struct Button;
    struct LayerTabs;
    struct ColorWell;
    struct TextRight;
    struct TextCopy;
    struct Toolbar;
    struct Stepper;
    struct Fader;
    struct Meter;
    struct Break;
    struct Edit;
    struct Text;
} } } }

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
        Window::Handle handle; // Ahora funciona porque Window::Handle está definido

        Aux(Window::Handle h) : handle(h) {}

        // Operadores PÚBLICOS
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
// WIDGETS REALES
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

// [FIX 2] Definiciones STUB para completar los tipos faltantes
struct Edit : Interface { static const char* class_() { return "EDIT"; } };
struct Text : Interface { static const char* class_() { return "STATIC"; } };
struct TextRight : Interface { static const char* class_() { return "STATIC"; } };
struct LayerTabs : Interface { static const char* class_() { return "SysTabControl32"; } }; 
struct Toolbar : Interface { static const char* class_() { return "ToolbarWindow32"; } };
struct Stepper : Interface { static const char* class_() { return "BUTTON"; } }; // Simplificación
struct Fader : Interface { static const char* class_() { return "STATIC"; } };
struct Meter : Interface { static const char* class_() { return "STATIC"; } };
struct Break : Interface { static const char* class_() { return "STATIC"; } };
struct TextCopy : Interface { static const char* class_() { return "STATIC"; } };

// [FIX 3] Estructura especial para ColorWell
struct ColorWell : Interface { 
    static const char* class_() { return "STATIC"; }
    // Soporte para la propiedad estática 'Type::custom' que usa el editor
    struct Type {
        static int& custom(int i) { 
            static int storage[32]; // Almacenamiento simple
            return storage[i]; 
        }
    };
};

// ............................................................................

} // ~ namespace widget
} // ~ namespace native
} // ~ namespace ui
} // ~ namespace kali

#endif // ~ KALI_UI_NATIVE_WIDGETS_INCLUDED
