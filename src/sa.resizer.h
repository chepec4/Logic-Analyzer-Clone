#ifndef SA_RESIZER_INCLUDED
#define SA_RESIZER_INCLUDED

#include "kali/ui/native.h"

namespace sa {

using namespace kali;

struct Resizer : ui::native::LayerBase
{
    int poll(HWND editor_) { return 0; }
    bool msgHook(LRESULT& result, UINT msg, WPARAM wparam, LPARAM lparam) { return false; }

    Resizer(AudioEffectX* plugin) : plugin(plugin) {}
    void close() override { }
    ~Resizer() { close(); }

private:
    AudioEffectX* plugin;
};

} // ~ namespace sa

// ============================================================================
// [C4 MASTER FIX] Especialización de Traits en el namespace original
// ============================================================================
namespace kali {
namespace details {

template <>
struct Traits <sa::Resizer> : TraitsBase <sa::Resizer>
{
    enum
    {
        styleEx = WS_EX_TRANSPARENT,
        style   = WS_POPUP
    };
};

} // ~ details
} // ~ kali

#endif
