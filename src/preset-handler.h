#ifndef PRESET_HANDLER_INCLUDED
#define PRESET_HANDLER_INCLUDED

#include "vst.h"
#include <stdio.h> // Para sprintf

// ............................................................................

template <int nPresets, int nParameters>
struct PresetBank
{
    int  count;
    int  index;
    char name  [nPresets][kVstMaxProgNameLen + 1]; // Ajustado a tamaño VST real
    int  value [nPresets][nParameters];
};

template <typename Plugin, int nPresets, int nParameters>
struct PresetHandler :
    vst::PluginBase <Plugin>,
    PresetBank <nPresets, nParameters>
{
    enum
    {
        PresetCount    = nPresets,
        ParameterCount = 1
    };

    virtual const int (&getPreset() const)[nParameters] = 0;
    virtual void setPreset(int (&)[nParameters])  = 0;

    void  invalidatePreset() { this->setParameterAutomated(0, 0); }

private:
    // Variables dummy para satisfacer la interfaz VST antigua
    float doom;
    float getParameter(VstInt32) {return doom;}
    void  setParameter(VstInt32, float v) {doom = v;}
    bool  canParameterBeAutomated(VstInt32) {return false;}
    
    // Usamos el macro 'copy' definido en main.h
    void  getParameterName(VstInt32, char* v) { copy(v, "None", 5); }
    void  getParameterDisplay(VstInt32 index, char* text) { sprintf(text, "%d", 0); } 
    void  getParameterLabel(VstInt32, char* text) { copy(text, "", 1); }

    // ........................................................................

    typedef vst::PluginBase <Plugin>            Base;
    typedef PresetBank <nPresets, nParameters> Bank;

    VstInt32 getProgram()
    {
        return this->index;
    }

    void setProgram(VstInt32 i)
    {
        this->index = i;
        setPreset(this->value[i]);
    }

    void setProgramName(char* text)
    {
        copy(this->name[this->index], text, kVstMaxProgNameLen);
    }

    void getProgramName(char* text)
    {
        copy(text, this->name[this->index], kVstMaxProgNameLen);
    }

    bool getProgramNameIndexed(VstInt32, VstInt32 i, char* text)
    {
        return (i < PresetCount)
            ? !!copy(text, this->name[i], kVstMaxProgNameLen)
            : false;
    }

    VstInt32 getChunk(void** data, bool isPreset)
    {
        // Implementación simplificada para evitar errores de compilación
        return 0;
    }

    VstInt32 setChunk(void* data_, VstInt32 size_, bool isPreset)
    {
        return 0;
    }

    // ........................................................................

public:

    template <typename Defaults>
    PresetHandler(audioMasterCallback master, const Defaults& defaults)
        : Base(master), doom(0.f)
    {
        this->programsAreChunks();

        this->count = nPresets;
        this->index = 0;
        for (int i = 0; i < nPresets; i++)
        {
            sprintf(this->name[i], "Program %d", i + 1);
            for(int j=0; j<nParameters; ++j) this->value[i][j] = defaults(j);
        }
    }
};

#endif
