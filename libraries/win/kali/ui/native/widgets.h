#ifndef KALI_UI_NATIVE_WIDGETS_INCLUDED
#define KALI_UI_NATIVE_WIDGETS_INCLUDED

#include "kali/ui/native.h"
#include "kali/ui/native/widgets.base.h"

namespace kali {
namespace ui {
namespace native {
namespace widget {

// ............................................................................
// ResourceCtor: Estructura crítica para la creación de la interfaz
// REPARACIÓN: Los operadores de conversión DEBEN ser públicos para que 
// el casting en sa.editor.h funcione correctamente.
// ............................................................................

struct ResourceCtor
{
    struct Aux
    {
        Window::Handle handle;

        Aux(Window::Handle h) : handle(h) {}

        // --- SECCIÓN PÚBLICA (Corregida para el build de GitHub) ---
    public:
        template <typename T> 
        operator T () const 
        { 
            return (T)handle; 
        }

        // Operador especializado para punteros de widgets kali
        template <typename T>
        operator T* () const
        {
            return (T*)handle;
        }
    };

    Aux operator () (int id) const
    {
        return Aux(::GetDlgItem(parent->handle, id));
    }

    const Window* parent;
    ResourceCtor(const Window* p) : parent(p) {}
};

// ............................................................................
// Definiciones de Widgets Estándar
// ............................................................................

struct Combo : Interface {
    static const uint32_t style_ = CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP;
    static const char* class_() { return "COMBOBOX"; }
    // ... resto de la implementación ...
};

struct Toggle : Interface {
    static const uint32_t style_ = BS_AUTOCHECKBOX | WS_TABSTOP;
    static const char* class_() { return "BUTTON"; }
    // ... resto de la implementación ...
};

// ............................................................................

} // ~ namespace widget
} // ~ namespace native
} // ~ namespace ui
} // ~ namespace kali

#endif // ~ KALI_UI_NATIVE_WIDGETS_INCLUDED
