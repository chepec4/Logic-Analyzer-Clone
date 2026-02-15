#ifndef PRESET_HANDLER_INCLUDED
#define PRESET_HANDLER_INCLUDED

#include "includes.h"
#include "kali/dbgutils.h"
#include "kali/runtime.h"
#include "vst/vst.h"
#include <algorithm>
#include <cstring>

/**
 * ESTRUCTURA DE DATOS DE PRESETS
 * Alineación de memoria garantizada para persistencia VST.
 */
template <int nPresets, int nParameters>
struct PresetBank
{
    int  count;
    int  index;
    // kVstMaxProgNameLen es usualmente 24, reservamos 28 para alineación de 4 bytes.
    char name  [nPresets][28];
    int  value [nPresets][nParameters];
};

/**
 * PRESET HANDLER - ARQUITECTURA PROTEGIDA
 * Gestiona el intercambio de programas y bancos entre el DAW y el Plugin.
 */
template <typename Plugin, int nPresets, int nParameters>
struct PresetHandler :
    vst::PluginBase <Plugin>,
    PresetBank <nPresets, nParameters>
{
    // [C4 FIX] Visibilidad de plantillas para GCC 12
    typedef vst::PluginBase <Plugin>      Base;
    typedef PresetBank <nPresets, nParameters> Bank;
    
    using Bank::count;
    using Bank::index;
    using Bank::name;
    using Bank::value;

    enum {
        PresetCount    = nPresets,
        ParameterCount = 1 // VST exige al menos 1 para automatización dummy
    };

    // Métodos virtuales puros que la clase Plugin debe implementar
    virtual const int (&getPreset() const)[nParameters] = 0;
    virtual void setPreset(int (&)[nParameters])  = 0;

    // Validación de seguridad para hosts que fallan con 0 parámetros
    void invalidatePreset() { this->setParameterAutomated(0, 0); }

private:
    // Gestión interna de parámetros VST (Dummy para compatibilidad)
    float getParameter(VstInt32) { return doom; }
    void  setParameter(VstInt32, float v) { doom = v; }
    bool  canParameterBeAutomated(VstInt32) { return false; }
    void  getParameterName(VstInt32, char* v) { this->copy(v, "None", 5); }

    // ........................................................................

    VstInt32 getProgram() { return index; }

    void setProgram(VstInt32 i) {
        if (i >= 0 && i < nPresets) {
            index = i;
            setPreset(value[i]);
        }
    }

    void setProgramName(char* text) {
        this->copy(name[index], text, kVstMaxProgNameLen);
    }

    void getProgramName(char* text) {
        this->copy(text, name[index], kVstMaxProgNameLen);
    }

    bool getProgramNameIndexed(VstInt32, VstInt32 i, char* text) {
        return (i >= 0 && i < PresetCount)
            ? !!this->copy(text, name[i], kVstMaxProgNameLen)
            : false;
    }

    // ........................................................................
    // PERSISTENCIA (CHUNKS)
    // ........................................................................

    VstInt32 getChunk(void** data, bool isPreset) {
        // Sincronizar preset actual antes de exportar
        std::memcpy(value[index], getPreset(), sizeof(value[index]));

        if (isPreset) {
            *data = value[index];
            return (VstInt32)sizeof(value[index]);
        }

        *data = &count; // Exporta el banco completo (PresetBank)
        return (VstInt32)sizeof(Bank);
    }

    VstInt32 setChunk(void* data_, VstInt32 size_, bool isPreset) {
        if (!data_ || size_ <= 0) return 0;

        // Caso 1: Carga de un solo preset (.fxp)
        if (isPreset) {
            int n = (int)(size_ / sizeof(int));
            n = std::min(n, (int)nParameters);
            const int* v = (const int*)data_;
            for (int j = 0; j < n; j++) value[index][j] = v[j];
            setPreset(value[index]);
            return 1;
        }

        // Caso 2: Carga de un banco completo (.fxb) - HARDENED LOGIC
        const char* rawData = (const char*)data_;
        const size_t totalSize = (size_t)size_;
        const size_t headerSize = sizeof(count) + sizeof(index);

        if (totalSize < headerSize) return 0;

        // Leer cabecera del banco
        int filePresetCount = *(const int*)rawData;
        if (filePresetCount <= 0) return 0;

        int fileIndex = *(const int*)(rawData + sizeof(count));
        
        // Calcular offsets con protección de desbordamiento
        const size_t namesTotalSize = (size_t)filePresetCount * sizeof(*name);
        const size_t valuesOffset = headerSize + namesTotalSize;

        if (valuesOffset > totalSize) return 0;

        // Determinar cuántos parámetros por preset hay en el archivo
        const size_t remainingBytes = totalSize - valuesOffset;
        const size_t rowByteSize = (size_t)filePresetCount * sizeof(int);
        
        if (rowByteSize == 0) return 0;
        int fileParamCount = (int)(remainingBytes / rowByteSize);

        // Punteros de lectura
        const char* namePtr = rawData + headerSize;
        const int* valuePtr = (const int*)(rawData + valuesOffset);

        // Copia selectiva respetando los límites del Plugin actual
        int m = std::min(filePresetCount, (int)nPresets);
        int n = std::min(fileParamCount, (int)nParameters);

        for (int i = 0; i < m; i++) {
            this->copy(name[i], namePtr, sizeof(*name));
            namePtr += sizeof(*name);
            for (int j = 0; j < n; j++) {
                value[i][j] = valuePtr[i * fileParamCount + j];
            }
        }

        index = (fileIndex >= 0 && fileIndex < nPresets) ? fileIndex : 0;
        setPreset(value[index]);

        return 1;
    }

public:
    template <typename Defaults>
    PresetHandler(audioMasterCallback master, const Defaults& defaults)
        : Base(master), doom(0.f)
    {
        // VST Opcode: el plugin gestiona su propio guardado/carga
        this->programsAreChunks();

        count = nPresets;
        index = 0;
        // Inicialización con valores por defecto (Factory Presets)
        for (int i = 0; i < nPresets; i++) {
            this->copy(name[i], defaults(i, value[i]), sizeof(*name));
        }
    }

private:
    float doom; // Parámetro dummy para compatibilidad de Host
};

#endif
