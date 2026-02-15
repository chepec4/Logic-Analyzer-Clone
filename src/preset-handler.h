#ifndef PRESET_HANDLER_INCLUDED
#define PRESET_HANDLER_INCLUDED

#include "vst.h"
#include <stdio.h>
#include <string.h>

/**
 * Estructura de Datos del Banco de Presets.
 * REGLA DE ORO: Los datos deben estar contiguos para permitir 
 * el guardado por "Chunks" (bloques binarios) de alta velocidad.
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
 * PresetHandler: Gestiona la persistencia de datos.
 * METICULOSIDAD: Implementa getChunk/setChunk para que el "Tilt" de Logic
 * y los colores de la UI se guarden correctamente.
 */
template <typename Plugin, int nPresets, int nParameters>
struct PresetHandler : vst::PluginBase <Plugin>, PresetBank <nPresets, nParameters>
{
    enum { PresetCount = nPresets };

    // Métodos virtuales que debe implementar la clase Plugin (main.h)
    virtual const int (&getPreset() const)[nParameters] = 0;
    virtual void setPreset(int (&)[nParameters])  = 0;

    // Notifica al DAW que un parámetro ha cambiado (vía automatización 0)
    void invalidatePreset() { this->setParameterAutomated(0, 0); }

private:
    float doom; // Parámetro dummy para cumplir con el estándar VST

    // --- Gestión de Parámetros VST ---
    float getParameter(VstInt32) { return doom; }
    void  setParameter(VstInt32, float v) { doom = v; }
    bool  canParameterBeAutomated(VstInt32) { return false; }
    
    // Meticulosidad: Usamos strncpy para asegurar integridad de strings
    void  getParameterName(VstInt32, char* v) { strncpy(v, "None", kVstMaxProgNameLen); }
    void  getParameterDisplay(VstInt32, char* text) { sprintf(text, "%d", 0); } 
    void  getParameterLabel(VstInt32, char* text) { text[0] = '\0'; }

    typedef vst::PluginBase <Plugin>           Base;
    typedef PresetBank <nPresets, nParameters> Bank;

    // --- Gestión de Programas (Presets) ---
    VstInt32 getProgram() { return this->index; }

    void setProgram(VstInt32 i) {
        if (i >= 0 && i < nPresets) {
            this->index = i;
            setPreset(this->value[i]);
        }
    }

    void setProgramName(char* text) {
        strncpy(this->name[this->index], text, kVstMaxProgNameLen);
        this->name[this->index][kVstMaxProgNameLen] = '\0'; // Terminador nulo forzado
    }

    void getProgramName(char* text) {
        strncpy(text, this->name[this->index], kVstMaxProgNameLen);
    }

    bool getProgramNameIndexed(VstInt32, VstInt32 i, char* text) {
        if (i < nPresets) {
            strncpy(text, this->name[i], kVstMaxProgNameLen);
            return true;
        }
        return false;
    }

    /**
     * SERIALIZACIÓN (Chunking): 
     * Permite guardar TODAS las bandas y configuraciones en un solo bloque.
     */
    VstInt32 getChunk(void** data, bool /*isPreset*/) {
        *data = (Bank*)this;
        return sizeof(Bank);
    }

    VstInt32 setChunk(void* data_, VstInt32 size_, bool /*isPreset*/) {
        if (size_ == sizeof(Bank)) {
            memcpy((Bank*)this, data_, sizeof(Bank));
            setPreset(this->value[this->index]);
            return 1;
        }
        return 0;
    }

public:
    /**
     * Constructor: Inicializa el banco con los valores por defecto.
     * EXTREMADAMENTE METICULOSO: Vincula el nombre del preset con su array de datos.
     */
    template <typename Defaults>
    PresetHandler(audioMasterCallback master, const Defaults& defaults)
        : Base(master), doom(0.f)
    {
        this->programsAreChunks(true); // Habilitamos guardado binario
        this->count = nPresets;
        this->index = 0;
        
        for (int i = 0; i < nPresets; i++) {
            // Se asume que defaults llena el array y devuelve el puntero al nombre
            const char* progName = defaults(i, this->value[i]);
            if (progName) {
                strncpy(this->name[i], progName, kVstMaxProgNameLen);
                this->name[i][kVstMaxProgNameLen] = '\0';
            }
        }
    }
};

#endif // ~ PRESET_HANDLER_INCLUDED
