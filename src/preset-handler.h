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

        if (!data_ || size_ <= 0)
            return 0;

        if (isPreset)
        {
            int n = size_ / int(sizeof(int));
            n = std::min(n, nParameters);
            const int* v = (const int*) data_;
            for (int j = 0; j < n; j++)
                value[index][j] = v[j];
            setPreset(value[index]);
            return 1;
        }

        const char* data = (const char*) data_;
        const size_t total = size_t(size_);

        const size_t header = sizeof(count) + sizeof(index);
        if (total < header)
            return 0;

        int m = *(const int*) data;
        if (m <= 0)
            return 0;

        index = *(const int*) (data + sizeof(count));

        const size_t namesSize = size_t(m) * sizeof(*name);
        if (namesSize / sizeof(*name) != size_t(m))
            return 0;

        const size_t valuesOffset = header + namesSize;
        if (valuesOffset > total)
            return 0;

        const char* text = data + header;

        const size_t elemSize = sizeof(**value);
        const size_t rowSize = size_t(m) * elemSize;
        if (!rowSize || rowSize / elemSize != size_t(m))
            return 0;

        const size_t valueBytes = total - valuesOffset;
        int n = int(valueBytes / rowSize);
        const int* v = (const int*) (data + valuesOffset);

        m = std::min(m, nPresets);
        n = std::min(n, nParameters);

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
