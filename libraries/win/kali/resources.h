#ifndef KALI_RESOURCES_INCLUDED
#define KALI_RESOURCES_INCLUDED

#include <windows.h>
#include "kali/dbgutils.h"

namespace kali     {
namespace resource {

// [C4 FIX] Helper para obtener DPI, faltante en la base original
inline int screenDPI() {
    HDC dc = ::GetDC(nullptr);
    int dpi = ::GetDeviceCaps(dc, LOGPIXELSY);
    ::ReleaseDC(nullptr, dc);
    return dpi ? dpi : 96;
}

// ............................................................................

template <typename T, bool Editable = true>
struct Raw
{
    T* data() const {return data_;}
    int size() const {return size_;}

    Raw(const char* type, const char* id, HMODULE module = app->module())
            : data_(nullptr), size_(0), handle(nullptr)
    {
        if (HRSRC rc = ::FindResource(module, id, type))
        {
            handle = ::LoadResource(module, rc);
            size_  = ::SizeofResource(module, rc);
            
            // Si es editable, hacemos una copia en el heap. Si no, usamos el puntero directo al recurso.
            if (Editable) {
                data_ = (T*) malloc(size_);
                if (data_) memcpy(data_, ::LockResource(handle), size_);
            } else {
                data_ = (T*) ::LockResource(handle);
            }
        }
        else {
            // trace("%s: FindResource(%s) failed [%i]\n", FUNCTION_, id, ::GetLastError());
        }
    }

    ~Raw()
    {
        if (Editable && data_)
            free(data_);
        // FreeResource es obsoleto en Win32 (no-op), pero lo mantenemos por formalidad si handle existe
        // if (handle) ::FreeResource(handle); 
    }

private:
    T* data_;
    int size_;
    HGLOBAL handle;
};

// ............................................................................
// Helpers de Fuentes e Iconos
// ............................................................................

// Parche en memoria para fuentes (truco legacy de Kali para ajustar DPI en recursos de fuentes binarias)
inline void font_(void* data, int size)
{
    if (!data || size < 20) return;
    
    char* p = (char*) data;
    static const wchar_t f1[] = L"MS Shell Dlg 2";
    static const wchar_t f2[] = L"Segoe UI";
    const int f1size = sizeof(f1) - sizeof(L"");
    const int f2size = sizeof(f2) - sizeof(L"");

    const int dpi = screenDPI();
    NONCLIENTMETRICSW ncm = {sizeof(ncm)};
    ::SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, 0, &ncm, 0);
    
    // Cálculo de altura ajustada a DPI
    int height = (-ncm.lfMessageFont.lfHeight * 72 + (dpi / 2)) / dpi;

    // Búsqueda simple de la cadena de fuente para reemplazarla
    while (--size > 0)
    {
        // Nota: Esto es un hack binario arriesgado, pero necesario para mantener 
        // la apariencia original si se usan recursos binarios precompilados.
        if (!memcmp(p, f1, f1size)) // Comparación de bytes crudos
        {
            // Ajustar altura en offset fijo relativo a la cadena
            *((short*) (p - 6)) = short(height);
            
            // Reemplazar nombre de fuente si el sistema usa Segoe UI
            if (!wcscmp(ncm.lfMessageFont.lfFaceName, f2))
            {
                memcpy(p, f2, sizeof(f2) - 2); // Copiar nombre sin null terminal extra si cabe
                // Mover el resto del buffer para ajustar tamaño si difieren
                memmove(p + f2size, p + f1size, size + 1 - f1size);
            }
            return;
        }
        ++p;
    }
}

} // ~ namespace resource
} // ~ namespace kali

#endif // ~ KALI_RESOURCES_INCLUDED
