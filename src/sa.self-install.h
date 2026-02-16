#ifndef SA_SELF_INSTALL_INCLUDED
#define SA_SELF_INSTALL_INCLUDED

#include "sa.settings.h"

namespace sa {

// ============================================================================
// INSTALADOR DE ESQUEMAS DE COLOR POR DEFECTO
// ============================================================================

void installColors()
{
    using namespace settings;

    // Formato: Nombre, [Colores ARGB...]
    struct Scheme { const char* name; int value[ColorsCount]; };
    
    static const Scheme scheme[] =
    {
        { "7th Fashion",       {0xabffffff, 0xe1f7b333, 0xabffffff, 0x54ffffff, 0x1cffffff, 0xabffffff, 0xff5a8296, 0xff1e4664} },
        { "Blues",             {0x666699ff, 0xeb6699ff, 0x7f66cc99, 0x99f3f3f3, 0x0ff3f3f3, 0x99f3f3f3, 0xff0c0c0c, 0xff33547d} },
        { "Brassica Oleracea", {0x5416a085, 0xa816a085, 0xffffffff, 0x5434495e, 0x1c34495e, 0xa834495e, 0xffd3ede9, 0xffc4e7e0} },
        { "Contrast",          {0xb300ff00, 0x99ffff00, 0xebff0000, 0x54ffffff, 0x1cffffff, 0x8fffffff, 0xff000000, 0xff000000} },
        { "Default",           {0xcc2080f0, 0x99ffffff, 0xebc0c0c0, 0x5effffff, 0x21ffffff, 0x8fffffff, 0xff303030, 0xff101010} },
        { "Electronics",       {0x5466cc99, 0xa833cc66, 0xa8ccff66, 0x54ffffff, 0x0fffffff, 0xa8ffffff, 0xff111312, 0xff345846} },
        { "Vegetable",         {0xffb2b8bc, 0xffffc53f, 0xffa5e346, 0xff828a8e, 0xff828a8e, 0xff828a8e, 0xff434c51, 0xff434c51} },
        { "Water In Charcoal", {0xcc2080f0, 0x99ffffff, 0xebc0c0c0, 0x5effffff, 0x21ffffff, 0x8fffffff, 0xff505050, 0xff101010} }
    };

    // Lógica de instalación en registro (Stub)
    // En una implementación completa, esto escribiría en HKEY_CURRENT_USER
}

} // ~ namespace sa

#endif
