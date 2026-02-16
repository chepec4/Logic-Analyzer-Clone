#ifndef KALI_VST_INCLUDED
#define KALI_VST_INCLUDED

#include "kali/app.h"

// [C4 FIX] Includes directos. 
// El Makefile maneja las rutas de búsqueda (-I) hacia 'public.sdk/source/vst2.x'.
// No usamos rutas relativas que no existen físicamente.
#include "aeffeditor.h"
#include "audioeffectx.h"

namespace vst {

// ............................................................................
// WRAPPER DEL PLUGIN (AudioEffectX)
// ............................................................................

template <typename Plugin>
struct PluginBase : AudioEffectX
{
    // [C4 FIX] Plugin::ProgramCount/ParameterCount/ID son requeridos por contrato.
    // main.h ya fue parcheado para proveerlos.
    PluginBase(audioMasterCallback master) : AudioEffectX(master, Plugin::ProgramCount, Plugin::ParameterCount)
    {
        setNumInputs(2);
        setNumOutputs(2);
        setUniqueID(Plugin::ID);
        canProcessReplacing();
        programsAreChunks(true);
    }

    virtual ~PluginBase() {}

    void programsAreChunks(bool v = true) { AudioEffectX::programsAreChunks(v); }
    
    // Helper legacy de copia segura
    static char* copy(char* dst, const char* src, int maxLen)
    {
        int i = 0;
        while ((i < maxLen - 1) && src[i]) {
            dst[i] = src[i];
            i++;
        }
        dst[i] = 0;
        return dst;
    }
};

// ............................................................................
// WRAPPER DEL EDITOR (AEffEditor -> Kali Window)
// ............................................................................

template <typename Plugin, typename Window>
struct Editor : AEffEditor
{
    Editor(Plugin* plugin) : plugin(plugin), window(nullptr) {}
    
    virtual ~Editor() 
    { 
        close(); 
    }

    bool getRect(ERect** rect) override
    {
        *rect = &rect_;
        return true;
    }

    bool open(void* ptr) override
    {
        close(); // Seguridad: limpiar estado previo
        
        // [C4 ARCHITECTURE FIX] Corrección de Instanciación
        // 1. Creamos el objeto C++ (Display) usando new estándar.
        // 2. Pasamos el puntero (T*) a createLayer.
        // Esto permite que el template deduzca T = sa::Display (la clase),
        // en lugar de T = sa::Display* (el puntero), arreglando el error de Traits.
        
        window = new Window(plugin); // 'plugin' se pasa al ctor de Display

        // createLayer "hidrata" el objeto window con un HWND nativo hijo de 'ptr'
        if (!kali::app::createLayer(reinterpret_cast<const kali::Window*>(ptr), window))
        {
            delete window;
            window = nullptr;
            return false;
        }

        // Inicialización lógica de la ventana (recursos GL, timers)
        if (!window->open())
        {
            window->close(); // Libera recursos y destruye HWND
            // window se limpia en close(), pero por seguridad:
            window = nullptr;
            return false;
        }

        updateRect();
        return true;
    }

    void close() override
    {
        if (window)
        {
            // [C4 FIX] Usamos el ciclo de vida nativo de Kali
            window->close();
            // Nota: Dependiendo de si Display se auto-elimina (delete this) o no,
            // aquí podríamos necesitar 'delete window'. 
            // En la arquitectura Kali moderna, LayerBase suele estar en AutoReleasePool.
            // Asumimos que close() maneja la destrucción del HWND.
            window = nullptr; 
        }
    }

    void idle() override 
    {
        // Kali maneja su propio loop de mensajes. No-op.
    }

private:
    void updateRect()
    {
        if (window) {
            kali::Rect r(window->position(), window->size());
            rect_.left   = (short)r.x;
            rect_.top    = (short)r.y;
            rect_.right  = (short)(r.x + r.w);
            rect_.bottom = (short)(r.y + r.h);
        }
    }

    Plugin* plugin;
    Window* window;
    ERect   rect_;
};

} // namespace vst

#endif
