#ifndef KALI_DBG_UTILS_INCLUDED
#define KALI_DBG_UTILS_INCLUDED

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "kali/platform.h"
#include "kali/details/noop.h"

// ............................................................................

// REPARACIÓN METICULOSA: Corrección de la lógica de detección de Debug
// 'defined' no es una función de runtime, es una directiva de pre-procesador.
#ifndef DBG
    #if defined(_DEBUG) || defined(DEBUG)
        #define DBG 1
    #else
        #define DBG 0
    #endif
#endif

#ifdef WINDOWS_
    #ifndef OutputDebugString
        extern "C" __declspec(dllimport)
            void __stdcall OutputDebugStringA(const char*);
    #endif
    // Si no estamos en Debug, NoOp neutraliza GetLastError para ahorrar ciclos
    #if !DBG
        #ifndef GetLastError
            #define GetLastError NoOp
        #endif
    #endif
#else
    #include <syslog.h>
#endif

// Macro para forzar una parada (Breakpoint manual)
#define DBGSTOP_ *((volatile int*) 0) = 0

// ............................................................................
// Estructura trace: Gestiona los logs del plugin

struct trace

#if !DBG
// En modo Release, hereda de NoOp para que el compilador elimine las llamadas
: NoOp
{
    NoOp warn, full, setLevel, setOutput, setPrefix;

#else // DBG (Modo Desarrollo activo)

{
    // Macro interna para simplificar el paso de argumentos variables
    #define out_helper(L, F)   \
        va_list A;      \
        va_start(A, F); \
        out(L, F, A);   \
        va_end(A);

    void operator () (const char* format, ...) const format__ { out_helper(Error, format); }
    void warn        (const char* format, ...) const format__ { out_helper(Warning, format); }
    void full        (const char* format, ...) const format__ { out_helper(Full, format); }

    #undef out_helper

    static void out(int level, const char* format, va_list args)
    {
        if (level > options().level)
            return;

        char msg[512];
        int m = options().prefixSize;
        int n = m;
        
        // Meticulosidad: Garantizar espacio para el prefijo y el sufijo "!!"
        const int size = sizeof(msg) - n - 4; 
        memcpy(msg, options().prefix, options().prefixSize);
        
        // Formateo seguro
        int written = vsnprintf(msg + n, size, format, args);
        if (written < 0) return; // Error de formato
        
        n += (written > size) ? size : written;

        // Si es un error crítico (< Warning), añadimos la marca visual
        if (level < Warning)
        {
            if (n > 0 && msg[n - 1] == '\n')
                strcpy(msg + n - 1, "!!\n");
            else
                strcpy(msg + n, "!!");
        }

        switch (options().output)
        {
            case Std: fputs(msg, stdout); return;
            case Err: fputs(msg, stderr); return;
            default: sysout(msg); return;
        }
    }

    static void setPrefix(const char* prefix)
    {
        char* p = options().prefix;
        int   n = sizeof(options().prefix) - 1;
        while ((n-- > 0) && *prefix)
            *p++ = *prefix++;
        *p = 0;
        options().prefixSize = (int)(p - options().prefix);
    }

    static void setLevel(int level)   { options().level  = level; }
    static void setOutput(int output) { options().output = output; }

    static void sysout(const char* msg)
    {
        #ifdef WINDOWS_
            OutputDebugStringA(msg);
        #else
            syslog(4, "%s", msg);
        #endif
    }

private:
    struct Options
    {
        int  level;
        int  output;
        char prefix[16];
        int  prefixSize;
    };

    static Options& options()
    {
        static Options aux = { Warning, Default, "", 0 };
        return aux;
    }

#endif // ~ DBG

public:
    enum { None = 0, Error, Warning, Full, Killing = None };
    enum { Sys = 0, Std, Err,
        #if MACOSX_
            #ifdef TRACE_OUTPUT
                Default = TRACE_OUTPUT
            #else
                Default = Err
            #endif
        #else
            Default = Sys
        #endif
    };

    trace() {}

} const trace;

// ............................................................................

#endif // ~ KALI_DBG_UTILS_INCLUDED
