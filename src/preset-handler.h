#ifndef PRESET_HANDLER_INCLUDED
#define PRESET_HANDLER_INCLUDED
#include "vst.h"
#include <stdio.h>

template <int nPresets, int nParameters>
struct PresetBank {
    int  count;
    int  index;
    char name  [nPresets][kVstMaxProgNameLen + 1];
    int  value [nPresets][nParameters];
};

template <typename Plugin, int nPresets, int nParameters>
struct PresetHandler : vst::PluginBase <Plugin>, PresetBank <nPresets, nParameters> {
    virtual const int (&getPreset() const)[nParameters] = 0;
    virtual void setPreset(int (&)[nParameters]) = 0;
    void invalidatePreset() { this->setParameterAutomated(0, 0); }
private:
    float doom;
    float getParameter(VstInt32) { return doom; }
    void  setParameter(VstInt32, float v) { doom = v; }
    bool  canParameterBeAutomated(VstInt32) { return false; }
    
    // Usamos bucle manual para evitar la restricción de strncpy
    static void safe_copy(char* dst, const char* src, int maxLen) {
        int i = 0;
        while (i < maxLen && src[i]) { dst[i] = src[i]; i++; }
        dst[i] = '\0';
    }

    void getParameterName(VstInt32, char* v) { safe_copy(v, "None", kVstMaxProgNameLen); }
    void getParameterDisplay(VstInt32, char* text) { sprintf(text, "%d", 0); }
    void getParameterLabel(VstInt32, char* text) { text[0] = '\0'; }

    typedef vst::PluginBase <Plugin> Base;
    typedef PresetBank <nPresets, nParameters> Bank;

    VstInt32 getProgram() { return this->index; }
    void setProgram(VstInt32 i) { if (i>=0 && i<nPresets) { this->index = i; setPreset(this->value[i]); } }
    void setProgramName(char* text) { safe_copy(this->name[this->index], text, kVstMaxProgNameLen); }
    void getProgramName(char* text) { safe_copy(text, this->name[this->index], kVstMaxProgNameLen); }
    bool getProgramNameIndexed(VstInt32, VstInt32 i, char* text) {
        if (i < nPresets) { safe_copy(text, this->name[i], kVstMaxProgNameLen); return true; }
        return false;
    }
    VstInt32 getChunk(void** data, bool b) { *data = (Bank*)this; return sizeof(Bank); }
    VstInt32 setChunk(void* d, VstInt32 s, bool b) { 
        if (s == sizeof(Bank)) { memcpy((Bank*)this, d, sizeof(Bank)); setPreset(this->value[this->index]); return 1; }
        return 0;
    }
public:
    template <typename Defaults>
    PresetHandler(audioMasterCallback m, const Defaults& d) : Base(m), doom(0.f) {
        this->programsAreChunks(true);
        this->count = nPresets; this->index = 0;
        for (int i = 0; i < nPresets; i++) {
            const char* pN = d(i, this->value[i]);
            if (pN) safe_copy(this->name[i], pN, kVstMaxProgNameLen);
        }
    }
};
#endif
