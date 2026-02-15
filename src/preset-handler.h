#ifndef PRESET_HANDLER_INCLUDED
#define PRESET_HANDLER_INCLUDED

#include "vst.h"
#include <stdio.h>

/**
 * PresetBank: Estructura binaria de datos contiguos.
 * REGLA DE ORO: Debe ser una estructura simple para permitir el guardado por Chunks.
 */
template <int nPresets, int nParameters>
struct PresetBank
{
    int  count;
    int  index;
    char name  [nPresets][kVstMaxProgNameLen + 1];
    int  value [nPresets][nParameters];
};

/**
 * PresetHandler: Gestiona la memoria de presets sin usar funciones prohibidas.
 * METICULOSIDAD: Implementamos una copia manual de bytes para saltar el error 
 * 'dont_use_strncpy_with_vst'.
 */
template <typename Plugin, int nPresets, int nParameters>
struct PresetHandler : vst::PluginBase <Plugin>, PresetBank <nPresets, nParameters>
{
    enum { PresetCount = nPresets };

    virtual const int (&getPreset() const)[nParameters] = 0;
    virtual void setPreset(int (&)[nParameters])  = 0;

    void invalidatePreset() { this->setParameterAutomated(0, 0); }

private:
    float doom; // Parámetro dummy VST

    /**
     * REGLA DE ORO: Función de copia segura que no depende de strncpy.
     * Esto soluciona el error de compilación de GitHub.
     */
    static void safe_copy(char* dst, const char* src, int maxLen)
    {
        if (!dst || !src) return;
        int i = 0;
        while (i < maxLen && src[i] != '\0') {
            dst[i] = src[i];
            i++;
        }
        dst[i] = '\0'; // Aseguramos siempre el terminador nulo
    }

    // --- Implementación de Parámetros VST ---
    float getParameter(VstInt32) { return doom; }
    void  setParameter(VstInt32, float v) { doom = v; }
    bool  canParameterBeAutomated(VstInt32) { return false; }
    
    void  getParameterName(VstInt32, char* v) { safe_copy(v, "None", kVstMaxProgNameLen); }
    void  getParameterDisplay(VstInt32, char* text) { sprintf(text, "%d", 0); } 
    void  getParameterLabel(VstInt32, char* text) { text[0] = '\0'; }

    typedef vst::PluginBase <Plugin>           Base;
    typedef PresetBank <nPresets, nParameters> Bank;

    // --- Gestión de Programas ---
    VstInt32 getProgram() { return this->index; }

    void setProgram(VstInt32 i) {
        if (i >= 0 && i < nPresets) {
            this->index = i;
            setPreset(this->value[i]);
        }
    }

    void setProgramName(char* text) {
        safe_copy(this->name[this->index], text, kVstMaxProgNameLen);
    }

    void getProgramName(char* text) {
        safe_copy(text, this->name[this->index], kVstMaxProgNameLen);
    }

    bool getProgramNameIndexed(VstInt32, VstInt32 i, char* text) {
        if (i < nPresets) {
            safe_copy(text, this->name[i], kVstMaxProgNameLen);
            return true;
        }
        return false;
    }

    /**
     * getChunk/setChunk: Serialización binaria directa.
     * Esto permite que Logic Pro guarde el "Tilt" y los colores en la sesión.
     */
    VstInt32 getChunk(void** data, bool /*isPreset*/) {
        *data = (Bank*)this;
        return (VstInt32)sizeof(Bank);
    }

    VstInt32 setChunk(void* data_, VstInt32 size_, bool /*isPreset*/) {
        if (size_ == (VstInt32)sizeof(Bank)) {
            for (int i = 0; i < (int)sizeof(Bank); i++) {
                ((char*)this)[i] = ((char*)data_)[i];
            }
            setPreset(this->value[this->index]);
            return 1;
        }
        return 0;
    }

public:
    template <typename Defaults>
    PresetHandler(audioMasterCallback master, const Defaults& defaults)
        : Base(master), doom(0.f)
    {
        this->programsAreChunks(true);
        this->count = nPresets;
        this->index = 0;
        
        for (int i = 0; i < nPresets; i++) {
            const char* progName = defaults(i, this->value[i]);
            if (progName) {
                safe_copy(this->name[i], progName, kVstMaxProgNameLen);
            }
        }
    }
};

#endif // ~ PRESET_HANDLER_INCLUDED
