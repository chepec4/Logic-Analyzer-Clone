#ifndef PRESET_HANDLER_INCLUDED
#define PRESET_HANDLER_INCLUDED

#include "includes.h"
#include "kali/dbgutils.h"
#include "kali/runtime.h"
#include "vst/vst.h"
#include <algorithm>
#include <cstring>

// ============================================================================
// ESTRUCTURA DE DATOS (Banco de Presets)
// ============================================================================

template <int nPresets, int nParameters>
struct PresetBank
{
    int  count;
    int  index;
    // VST 2.4 define kVstMaxProgNameLen como 24. 
    // Usamos 28 para padding/alineación segura en memoria.
    char name  [nPresets][28];
    int  value [nPresets][nParameters];
};

// ============================================================================
// GESTOR DE PRESETS (Chunks VST)
// ============================================================================

template <typename Plugin, int nPresets, int nParameters>
struct PresetHandler :
    vst::PluginBase <Plugin>,
    PresetBank <nPresets, nParameters>
{
    typedef vst::PluginBase <Plugin>           Base;
    typedef PresetBank <nPresets, nParameters> Bank;
    
    // Visibilidad de miembros de la clase base template
    using Bank::count;
    using Bank::index;
    using Bank::name;
    using Bank::value;

    // --- Implementación de VST Base ---

    void setProgram(VstInt32 i) override {
        if (i >= 0 && i < count) {
            index = i;
            // setPreset debe estar definido en la clase derivada 'Plugin'
            static_cast<Plugin*>(this)->setPreset(value[index]);
        }
    }

    void setProgramName(char* s) override {
        this->copy(name[index], s, sizeof(*name));
    }

    void getProgramName(char* s) override {
        this->copy(s, name[index], sizeof(*name));
    }

    bool getProgramNameIndexed(VstInt32 cat, VstInt32 i, char* s) override {
        if (i >= 0 && i < count) {
            this->copy(s, name[i], sizeof(*name));
            return true;
        }
        return false;
    }

    // --- Persistencia (Chunks) ---

    // Obtener chunk (Guardar proyecto)
    VstInt32 getChunk(void** data, bool isPreset) override {
        // En VST 2.4, los chunks opacos suelen ser del banco completo
        *data = static_cast<Bank*>(this);
        return sizeof(Bank);
    }

    // Cargar chunk (Abrir proyecto)
    VstInt32 setChunk(void* data, VstInt32 byteSize, bool isPreset) override {
        if (byteSize != sizeof(Bank)) {
            // [C4 SAFETY] Validación básica de tamaño para evitar corrupción
            // Si el tamaño difiere, podríamos intentar una migración parcial aquí,
            // pero por seguridad retornamos fallo si no coincide la estructura.
            return 0;
        }
        
        // Carga directa de memoria
        std::memcpy(static_cast<Bank*>(this), data, sizeof(Bank));
        
        // Restaurar estado actual
        setProgram(index);
        return 1;
    }

    // --- API Pública del Handler ---

    // Notificar cambio de parámetro desde UI o Host
    void invalidatePreset() {
        // Copiar los valores "vivos" del plugin al almacenamiento del preset actual
        const int (&currentValues)[nParameters] = static_cast<Plugin*>(this)->getPreset();
        for (int i = 0; i < nParameters; ++i) {
            value[index][i] = currentValues[i];
        }
    }

protected:
    template <typename Defaults>
    PresetHandler(audioMasterCallback master, const Defaults& defaults)
        : Base(master) // Inicializa AudioEffectX
    {
        // Indicar al host que usamos Chunks para persistencia total
        this->programsAreChunks(true);

        count = nPresets;
        index = 0;

        // Inicialización de fábrica
        for (int i = 0; i < nPresets; ++i) {
            const char* defaultName = defaults(i, value[i]);
            this->copy(name[i], defaultName, sizeof(*name));
        }
    }
    
    // Destructor virtual requerido por herencia
    virtual ~PresetHandler() {}
};

#endif
