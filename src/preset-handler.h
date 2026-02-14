#ifndef PRESET_HANDLER_INCLUDED
#define PRESET_HANDLER_INCLUDED

// Incluimos librerías estándar para reemplazar las de kali que fallan
#include <string.h> // Para memcpy
#include <algorithm> // Para std::min
#include <stdio.h>   // Para sprintf
#include "vst/vst.h"

// ............................................................................

// Definimos un reemplazo local para 'copy' usando memcpy estándar
inline void* my_copy(void* dest, const void* src, size_t count) {
    return memcpy(dest, src, count);
}

// Macro para anular el trace (logs) y evitar errores de kali/dbgutils
#define TRACE_FULL(...) 

// ............................................................................

template <int nPresets, int nParameters>
struct PresetBank
{
    int  count;
    int  index;
    char name  [nPresets][28];
    int  value [nPresets][nParameters];
};

template <typename Plugin, int nPresets, int nParameters>
struct PresetHandler :
    vst::PluginBase <Plugin>,
    PresetBank <nPresets, nParameters>
{
    typedef vst::PluginBase <Plugin> Base;
    typedef PresetBank <nPresets, nParameters> Bank;

    enum
    {
        PresetCount    = nPresets,
        ParameterCount = 1
    };

    virtual const int (&getPreset() const)[nParameters] = 0;
    virtual void setPreset(int (&)[nParameters])  = 0;

    // Corrección: Usamos 'this->' para acceder a setParameterAutomated de la clase base
    void  invalidatePreset() { this->setParameterAutomated(0, 0); }

private:

    float getParameter(VstInt32) {return doom;}
    void  setParameter(VstInt32, float v) {doom = v;}
    bool  canParameterBeAutomated(VstInt32) {return false;}
    
    // Corrección: Usamos my_copy en lugar de copy
    void  getParameterName(VstInt32, char* v) { my_copy(v, "None", 5); }

    // ........................................................................

    VstInt32 getProgram()
    {
        // Corrección: Acceso a miembro de clase base con 'this->'
        return this->index;
    }

    void setProgram(VstInt32 i)
    {
        TRACE_FULL("%s(%i)\n", __FUNCTION__, i);
        this->index = i;
        setPreset(this->value[i]);
    }

    void setProgramName(char* text)
    {
        my_copy(this->name[this->index], text, kVstMaxProgNameLen);
    }

    void getProgramName(char* text)
    {
        my_copy(text, this->name[this->index], kVstMaxProgNameLen);
    }

    bool getProgramNameIndexed(VstInt32, VstInt32 i, char* text)
    {
        return (i < PresetCount)
            ? !!my_copy(text, this->name[i], kVstMaxProgNameLen)
            : false;
    }

    VstInt32 getChunk(void** data, bool isPreset)
    {
        TRACE_FULL("%s: %s (%p)\n", __FUNCTION__, isPreset ? "Preset" : "Bank", this->value[this->index]);

        memcpy(this->value[this->index], getPreset(), sizeof(*this->value));

        if (isPreset)
        {
            *data = this->value[this->index];
            return sizeof(*this->value);
        }

        *data = &this->count;
        return sizeof(Bank);
    }

    VstInt32 setChunk(void* data_, VstInt32 size_, bool isPreset)
    {
        TRACE_FULL("%s: %s, (%p) size %i\n", __FUNCTION__, isPreset ? "Preset" : "Bank", data_, size_);

        if (isPreset)
        {
            int n = size_ / sizeof(int);
            // Corrección: Usamos std::min en lugar de kali::min
            n = std::min(n, nParameters);
            const int* v = (const int*) data_;
            for (int j = 0; j < n; j++)
                this->value[this->index][j] = v[j];
            setPreset(this->value[this->index]);
            return 1;
        }

        int size = 0;
        const char* data = (const char*) data_;
        int m = *(const int*) data;
        size += sizeof(this->count);
        this->index = *(const int*) (data + size);
        size += sizeof(this->count);
        const char* text = data + size;
        size += m * sizeof(*this->name);
        int n = (size_ - size) / (m * sizeof(**this->value));
        const int* v = (const int*) (data + size);

        m = std::min(m, nPresets); // std::min
        n = std::min(n, nParameters); // std::min

        for (int i = 0; i < m; i++)
        {
            my_copy(this->name[i], text, sizeof(*this->name));
            text += sizeof(*this->name);
            for (int j = 0; j < n; j++)
                this->value[i][j] = *v++;
        }

        if (this->index < 0 || this->index >= nPresets)
            this->index = 0;
        setPreset(this->value[this->index]);

        return 1;
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
            my_copy(this->name[i], defaults(i, this->value[i]), sizeof(*this->name));
    }

private:
    float doom;
};

// ............................................................................

#endif // ~ PRESET_HANDLER_INCLUDED
