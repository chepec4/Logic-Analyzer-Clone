#ifndef SA_WIDGETS_INCLUDED
#define SA_WIDGETS_INCLUDED

#include "kali/window.h"
#include "sa.resizer.h"

namespace sa {

using namespace kali;

/**
 * RASTREADOR DE POSICIÓN DE MOUSE (Logic Pro Precision)
 * Esta plantilla añade capacidades de tracking a cualquier capa visual (LayerBase).
 * Es fundamental para el cálculo del 'Pointer Info' (Freq/dB).
 */
template <typename Base>
struct TrackMousePosition : Base
{
    // Constructor inicializa el mouse fuera de rango
    TrackMousePosition() : mousePos(-1, -1) {}

    // [C4 FIX] Gestión de movimiento con re-activación de tracking
    bool mouseMove(int x, int y)
    {
        // Si el mouse estaba fuera, habilitamos el evento de salida de Win32
        if (mousePos.x < 0) {
            enableMouseLeave();
        }
        
        // Actualizamos coordenadas usando el tipo consolidado de window.h
        mousePos = Point(x, y);
        
        // Invalida la zona para redibujar el Pointer Info de Logic
        this->invalidate(false);
        return true;
    }

    // [C4 FIX] Implementación obligatoria para evitar 'Pointer info' huérfano
    bool mouseLeave()
    {
        // Invalidamos la posición para que sa::Display deje de dibujar el cuadro
        mousePos = Point(-1, -1);
        this->invalidate(false);
        return true;
    }

private:

    /**
     * Activa el servicio de notificación del sistema para cuando el mouse
     * sale del área del plugin. Es vital para la estabilidad de la UI.
     */
    void enableMouseLeave() const
    {
        TRACKMOUSEEVENT tme;
        tme.cbSize      = sizeof(tme);
        tme.dwFlags     = TME_LEAVE;
        // [C4 FIX] 'this->handle' es obligatorio en plantillas dependientes
        tme.hwndTrack   = this->handle;
        tme.dwHoverTime = HOVER_DEFAULT;
        
        ::TrackMouseEvent(&tme);
    }

protected:
    // Coordenadas actuales del cursor relativas a la rejilla del analizador
    Point mousePos;
};

} // ~ namespace sa

#endif // ~ SA_WIDGETS_INCLUDED
