#ifndef KALI_UI_NATIVE_INCLUDED
#define KALI_UI_NATIVE_INCLUDED

#include "kali/app.h"
#include "kali/ui/native/widgets.h"

// ............................................................................

namespace kali   {
namespace ui     {
namespace native {

// ............................................................................

/**
 * TYPEDEFS DE ACCESO RÁPIDO:
 * Meticulosidad: Definimos alias para los Ptr de cada widget.
 * Esto asegura que sa.display.h pueda declarar elementos de UI
 * sin preocuparse por la gestión de delete manual.
 */
typedef Ptr <widget::Interface> AnyWidget;

typedef Ptr <widget::LayerTabs> LayerTabs;
typedef Ptr <widget::ColorWell> ColorWell;
typedef Ptr <widget::TextRight> TextRight;
typedef Ptr <widget::TextCopy>  TextCopy;
typedef Ptr <widget::Toolbar>   Toolbar;
typedef Ptr <widget::Stepper>   Stepper;
typedef Ptr <widget::Button>    Button;
typedef Ptr <widget::Toggle>    Toggle;
typedef Ptr <widget::Fader>     Fader;
typedef Ptr <widget::Meter>     Meter;
typedef Ptr <widget::Combo>     Combo;
typedef Ptr <widget::Break>     Break;
typedef Ptr <widget::Edit>      Edit;
typedef Ptr <widget::Text>      Text;

// ............................................................................

/**
 * LayerBase: La clase base de nuestra visualización.
 * REGLA DE ORO #2: sa::Display hereda de aquí. 
 * Proporciona el sistema de dibujo y eventos de ratón para el análisis.
 */
struct LayerBase : Window, ui::Layer, widget::Parent
{
    enum style       {SysCaption, DropShadow = 0};
    
    // Implementación de widget::Parent: la capa es su propia ventana
    Window* window() {return this;}
    
    // Ciclo de vida de la capa
    virtual bool open()  {return true;}
    virtual void close() {}

    // --- INTERFAZ DE EVENTOS (Virtuales para sa::Display) ---
    // Meticulosidad: Deben ser virtuales para que sa::Display::draw() sea llamado.
    virtual bool draw()                {return false;}
    virtual void resized()             {}
    virtual bool mouseDoubleClick()    {return false;}
    virtual bool mouseMove(int, int)   {return false;}
    virtual bool keyDown(int, int)     {return false;}
    virtual bool mouse(int, int, int)  {return false;}
    virtual bool mouseR(int, int, int) {return false;}

    // Hook de mensajes de Windows para capturar eventos de bajo nivel
    virtual bool msgHook(LRESULT&, UINT, WPARAM, LPARAM) {return false;}

    /**
     * Destructor: Limpieza automática de la memoria de la UI.
     * EXTREMADAMENTE METICULOSO: Llama a destroy() de Window para 
     * cerrar el contexto de OpenGL antes de que el objeto desaparezca.
     */
    virtual ~LayerBase() { this->destroy(); }

private:
    // Vincula la gestión de memoria automática (AutoRelease)
    void attach(ReleaseAny* any) { autorelease(any); }

protected:
    // Pool de liberación automática: evita memory leaks en el DAW.
    AutoReleasePool<> autorelease;
};

// ............................................................................

/**
 * WindowBase: Para ventanas emergentes (como el Editor de Ajustes).
 */
struct WindowBase : LayerBase
{
    enum style {SysCaption = 1};

    virtual bool open()
    {
        LayerBase::open();
        centerAt(); // Centra la ventana automáticamente al abrir
        return true;
    }

    /**
     * Centra la ventana respecto a la pantalla o a otra ventana (DAW).
     */
    void centerAt(const Window* at = 0)
    {
        Rect r = !at ? Rect(screenSize())
            : Rect(at->position(), at->size());
        Size s = Window::size();
        r.x += (r.w - s.w) / 2;
        r.y += (r.h - s.h) / 2;
        Window::position(r.x, r.y);
    }
};

// ............................................................................

} // ~ namespace native
} // ~ namespace ui
} // ~ namespace kali

// ............................................................................

#endif // ~ KALI_UI_NATIVE_INCLUDED
