#ifndef SA_WIDGETS_INCLUDED
#define SA_WIDGETS_INCLUDED

#include "kali/window.h"
#include "sa.resizer.h"

namespace sa {

using namespace kali;

// ============================================================================
// MIXIN: TRACKING DE POSICIÓN DE RATÓN
// ============================================================================
// Añade capacidad de detectar MouseLeave a cualquier ventana/layer base.

template <typename Base>
struct TrackMousePosition : Base
{
    // Coordenada local del mouse (-1, -1 indica fuera)
    Point mousePos;

    TrackMousePosition() : mousePos(-1, -1) {}

    // Callback de movimiento (debe ser invocado desde el hook de mensajes o dispatch)
    bool mouseMove(int x, int y)
    {
        // Si el mouse estaba "fuera", reactivamos el rastreo de salida
        if (mousePos.x < 0) {
            enableMouseLeave();
        }
        
        mousePos = Point(x, y);
        
        // Invalidamos (false = sin borrar fondo) para redibujar la info
        this->invalidate(false);
        return true;
    }

    // Callback de salida (Mouse Leave)
    bool mouseLeave()
    {
        mousePos = Point(-1, -1);
        this->invalidate(false);
        return true;
    }

private:
    // Activa la notificación WM_MOUSELEAVE en Windows
    void enableMouseLeave() const
    {
        TRACKMOUSEEVENT tme;
        tme.cbSize      = sizeof(tme);
        tme.dwFlags     = TME_LEAVE;
        // [C4 FIX] Acceso explícito a 'handle' dependiente de la clase base
        tme.hwndTrack   = this->handle; 
        tme.dwHoverTime = HOVER_DEFAULT;
        
        // Ignoramos error si falla, no es crítico
        ::TrackMouseEvent(&tme);
    }
};

} // ~ namespace sa

#endif
