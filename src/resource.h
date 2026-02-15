#ifndef RESOURCE_INCLUDED
#define RESOURCE_INCLUDED

// 1. ICONOS
#define IDI_ICON1                       101

// 2. BITMAPS (Sincronizados con tu carpeta src/rc)
// Estos IDs son los que buscará la clase sa::Display
#define IDB_TOOLBAR_OFF                 102
#define IDB_TOOLBAR_ON                  103

// 3. DIÁLOGOS (Definidos en dialogs.rc)
#define IDD_EDITOR                      104
#define IDD_SETTINGS                    105

// 4. CONTROLES (Utilizados en sa.editor.h)
#define IDC_COMBO_BANDS                 1001
#define IDC_CHECK_PEAK                  1002
#define IDC_CHECK_HOLD                  1003
#define IDC_CHECK_AVRG                  1004
#define IDC_COLOR_WELL                  1005

// Definiciones de control de Microsoft (Mantener para compatibilidad)
#ifdef APSTUDIO_INVOKED
#ifndef APSTUDIO_READONLY_SYMBOLS
#define _APS_NEXT_RESOURCE_VALUE        107
#define _APS_NEXT_COMMAND_VALUE         40003
#define _APS_NEXT_CONTROL_VALUE         1006
#define _APS_NEXT_SYMED_VALUE           100
#endif
#endif

#endif // RESOURCE_INCLUDED
