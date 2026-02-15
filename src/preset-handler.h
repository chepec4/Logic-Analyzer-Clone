#ifndef PRESET_HANDLER_INCLUDED
#define PRESET_HANDLER_INCLUDED

#include "includes.h"
#include "kali/dbgutils.h"
#include "kali/runtime.h"
#include "vst/vst.h"
#include <algorithm> // Para std::min

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
    // [C4 FIX] Importamos tipos y miembros de las clases base para que GCC los vea
    typedef vst::PluginBase <Plugin>       Base;
    typedef PresetBank <nPresets, nParameters> Bank;
    
    using Bank::count;
    using Bank::index;
    using Bank::name;
    using Bank::value;
    // 'copy' viene de PluginBase (probablemente kali/utils o similar)
    // Como es dependiente, usaremos this->copy o kali::copy si existe.

    enum
    {
        PresetCount    = nPresets,
        ParameterCount = 1
    };

    virtual const int (&getPreset() const)[nParameters] = 0;
    virtual void setPreset(int (&)[nParameters])  = 0;

    // Workaround para hosts que no soportan 0 parámetros
    void  invalidatePreset() { this->setParameterAutomated(0, 0); }

private:

    float getParameter(VstInt32) {return doom;}
    void  setParameter(VstInt32, float v) {doom = v;}
    bool  canParameterBeAutomated(VstInt32) {return false;}
    
    // [C4 FIX] this->copy
    void  getParameterName(VstInt32, char* v) { this->copy(v, "None", 5); }

    // ........................................................................

    VstInt32 getProgram()
    {
        return index;
    }

    void setProgram(VstInt32 i)
    {
        trace.full("%s(%i)\n", FUNCTION_, i);
        if (i >= 0 && i < nPresets) // [C4 FIX] Protección de límites
        {
            index = i;
            setPreset(value[i]);
        }
    }

    void setProgramName(char* text)
    {
        this->copy(name[index], text, kVstMaxProgNameLen);
    }

    void getProgramName(char* text)
    {
        this->copy(text, name[index], kVstMaxProgNameLen);
    }

    bool getProgramNameIndexed(VstInt32, VstInt32 i, char* text)
    {
        return (i < PresetCount)
            ? !!this->copy(text, name[i], kVstMaxProgNameLen)
            : false;
    }

    VstInt32 getChunk(void** data, bool isPreset)
    {
        trace.full("%s: %s (%p)\n", FUNCTION_,
            isPreset ? "Preset" : "Bank", value[index]);

        // [C4 FIX] memcpy seguro
        memcpy(value[index], getPreset(), sizeof(value[index]));

        if (isPreset)
        {
            *data = value[index];
            return sizeof(value[index]);
        }

        *data = &count; // count es el primer miembro de Bank
        return sizeof(Bank);
    }

    VstInt32 setChunk(void* data_, VstInt32 size_, bool isPreset)
    {
        trace.full("%s: %s, (%p) size %i\n", FUNCTION_,
            isPreset ? "Preset" : "Bank", data_, size_);

        if (isPreset)
        {
            int n = size_ / sizeof(int);
            n = std::min(n, nParameters); // [C4 FIX] std::min
            const int* v = (const int*) data_;
            for (int j = 0; j < n; j++)
                value[index][j] = v[j];
            setPreset(value[index]);
            return 1;
        }

        int size = 0;
        const char* data = (const char*) data_;
        int m = *(const int*) data;
        
        // Ajuste de offsets basado en estructura Bank
        size += sizeof(count);
        
        // [C4 FIX] Validación de punteros antes de leer
        if (size + sizeof(index) > (size_t)size_) return 0;
        
        index = *(const int*) (data + size);
        size += sizeof(count); // index ocupa sizeof(int) == sizeof(count)
        
        const char* text = data + size;
        size += m * sizeof(*name);
        
        // Validación de división por cero
        size_t elemSize = sizeof(**value); // sizeof(int)
        if (elemSize == 0) return 0;
        
        int n = (size_ - size) / (m * elemSize);
        const int* v = (const int*) (data + size);

        m = std::min(m, nPresets);    // [C4 FIX] std::min
        n = std::min(n, nParameters); // [C4 FIX] std::min

        for (int i = 0; i < m; i++)
        {
            this->copy(name[i], text, sizeof(*name));
            text += sizeof(*name);
            for (int j = 0; j < n; j++)
                value[i][j] = *v++;
        }

        if (index < 0 || index >= nPresets)
            index = 0;
        
        setPreset(value[index]);

        return 1;
    }

    // ........................................................................

public:

    template <typename Defaults>
    PresetHandler(audioMasterCallback master, const Defaults& defaults)
        : Base(master), doom(0.f)
    {
        this->programsAreChunks();

        count = nPresets;
        index = 0;
        for (int i = 0; i < nPresets; i++)
            this->copy(name[i], defaults(i, value[i]), sizeof(*name));
    }

private:
    float doom;
};

// ............................................................................

#endif // ~ PRESET_HANDLER_INCLUDED
