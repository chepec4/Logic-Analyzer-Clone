#ifndef KALI_NOOP_INCLUDED
#define KALI_NOOP_INCLUDED

// ............................................................................

struct NoOp
{
    // [C4 OPTIMIZATION] Modernización a Variadic Templates (C++17)
    // Esto reemplaza las 8 sobrecargas manuales anteriores.
    // Acepta cualquier cantidad de argumentos de cualquier tipo y no hace nada.
    // El compilador optimizará estas llamadas eliminándolas completamente (Dead Code Elimination).
    
    template <typename... Args>
    void operator () (Args&&...) const {}
};

// ............................................................................

#endif // ~ KALI_NOOP_INCLUDED
