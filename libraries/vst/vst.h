#ifndef KALI_VST_INCLUDED
#define KALI_VST_INCLUDED

#include "kali/app.h"
#include "vst/pluginterfaces/vst2.x/aeffeditor.h"
#include "vst/pluginterfaces/vst2.x/audioeffectx.h"

namespace vst {

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

    // Helpers de seguridad VST
    void programsAreChunks(bool v = true) { AudioEffectX::programsAreChunks(v); }
    
    // Copia segura de strings
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

template <typename Plugin, typename Window>
struct Editor : AEffEditor
{
    Editor(Plugin* plugin) : plugin(plugin), window(0) {}
    
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
        close(); // Seguridad: cerrar anterior si existe
        
        // [C4 ARCHITECTURE FIX] Uso del sistema de ventanas Kali moderno
        if (!kali::app::createLayer(reinterpret_cast<const kali::Window*>(ptr), &window))
            return false;

        // Inyección de dependencia del plugin
        new (window) Window(plugin);

        if (!window->open())
        {
            // [C4 FIX] close() en lugar de destroy()
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
            // [C4 ARCHITECTURE FIX] Alineación con LayerBase::close()
            window->close();
            // Kali gestiona la memoria, pero limpiamos el puntero
            window = 0; 
        }
    }

    void idle() override 
    {
        // Hook para tareas en segundo plano si fuera necesario
    }

private:
    void updateRect()
    {
        // [C4 FIX] Uso de geometría moderna
        kali::Rect r(window->position(), window->size());
        rect_.left   = (short)r.x;
        rect_.top    = (short)r.y;
        rect_.right  = (short)(r.x + r.w);
        rect_.bottom = (short)(r.y + r.h);
    }

    Plugin* plugin;
    Window* window;
    ERect   rect_;
};

} // namespace vst

#endif
