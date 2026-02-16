#ifndef KALI_SETTINGS_INCLUDED
#define KALI_SETTINGS_INCLUDED

#include <windows.h>
#include "kali/dbgutils.h"

namespace kali {

struct Settings
{
    void set(const char* name, int value) const
    {
        ::RegSetValueEx(handle, name, 0, REG_DWORD, (BYTE*) &value, sizeof(value));
    }

    int get(const char* name, int default_) const
    {
        DWORD size = sizeof(default_);
        DWORD type = REG_DWORD;
        LONG res = ::RegQueryValueEx(handle, name, 0, &type, (BYTE*) &default_, &size);
        return (res == ERROR_SUCCESS) ? default_ : default_;
    }

    void set(const char* name, const char* value) const
    {
        if (value)
            ::RegSetValueEx(handle, name, 0, REG_SZ, (const BYTE*) value, (DWORD) strlen(value) + 1);
    }

    #ifdef KALI_STRING_INCLUDED
    kali::string get(const char* name, const char* default_) const
    {
        char buffer[1024]; // Buffer fijo seguro
        DWORD size = sizeof(buffer);
        DWORD type = REG_SZ;
        LONG res = ::RegQueryValueEx(handle, name, 0, &type, (BYTE*) buffer, &size);
        
        if (res == ERROR_SUCCESS) return kali::string("%s", buffer);
        return kali::string("%s", default_);
    }
    #endif

    // ........................................................................

    ~Settings() {
        if (handle) ::RegCloseKey(handle);
    }

    Settings(const char* key)
        : handle(ctor(HKEY_CURRENT_USER, key)) {}

    Settings(const Settings& root, const char* key)
        : handle(ctor(root.handle, key)) {}

private:
    typedef ::HKEY Handle;
    Handle handle;
    
    // Deshabilitar copia
    Settings(const Settings&);
    Settings& operator = (const Settings&);

    static Handle ctor(Handle root, const char* key)
    {
        Handle handle = nullptr;
        // Intentamos abrir o crear la clave
        DWORD disp;
        LONG res = ::RegCreateKeyEx(root, key, 0, nullptr, 
            REG_OPTION_NON_VOLATILE, 
            KEY_READ | KEY_WRITE, 
            nullptr, &handle, &disp);
            
        if (res != ERROR_SUCCESS) {
            // trace("RegCreateKeyEx failed: %d\n", res);
            return nullptr;
        }
        return handle;
    }
};

} // ~ namespace kali

#endif // ~ KALI_SETTINGS_INCLUDED
