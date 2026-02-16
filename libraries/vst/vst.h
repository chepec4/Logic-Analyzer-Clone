#ifndef KALI_VST_INCLUDED
#define KALI_VST_INCLUDED

#include "kali/app.h"

// [C4 ARCHITECTURE FIX]
// El Makefile (V11.0) ya incluye las rutas:
// -I../libraries/vst/public.sdk/source/vst2.x
// Por lo tanto, la inclusión debe ser directa, sin prefijos de ruta inexistentes.
#include "aeffeditor.h"
#include "audioeffectx.h"

namespace vst {

// ............................................................................
// CLASE BASE DEL PLUGIN (Wrapper de AudioEffectX)
// ............................................................................

template <typename Plugin>
struct PluginBase : AudioEffectX
{
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
    
    // Copia segura de strings para VST (Legacy Support)
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
// EDITOR DEL PLUGIN (Puente entre VST Host y Kali Window)
// ............................................................................

template <typename Plugin, typename Window>
struct Editor : AEffEditor
{
    // [C4 FIX] Inicialización con nullptr (C++17)
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
        close(); // Seguridad: cerrar instancia previa si existe
        
        // [C4 ARCHITECTURE] Kali App crea la capa base sobre el HWND del Host
        // El casting a const kali::Window* es seguro, Kali extrae el HWND internamente.
        if (!kali::app::createLayer(reinterpret_cast<const kali::Window*>(ptr), &window))
            return false;

        // "Placement New": Construimos la ventana (sa::Display) en la memoria asignada
        new (window) Window(plugin);

        if (!window->open())
        {
            // [C4 FIX] Usamos close() en lugar de destroy() para cumplir el Codex
            window->close(); 
            return false;
        }

        updateRect();
        return true;
    }

    void close() override
    {
        if (window)
        {
            // [C4 ARCHITECTURE] Delegamos el cierre a Kali (::DestroyWindow)
            window->close();
            window = nullptr; 
        }
    }

    void idle() override 
    {
        // El framework Kali maneja su propio loop de mensajes (Timer/Hooks)
        // No requerimos bombeo manual desde el Host VST
    }

private:
    void updateRect()
    {
        // Sincronización de geometría Kali -> VST ERect
        if (window) {
            // [C4 FIX] Uso de métodos modernos de Window (position/size)
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
