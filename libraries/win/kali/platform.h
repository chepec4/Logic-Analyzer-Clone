#ifndef KALI_PLATFORM_INCLUDED
#define KALI_PLATFORM_INCLUDED

// ............................................................................

#define WINDOWS_ 1

// DLL Export
#if defined(_MSC_VER)
    #define API_EXPORT __declspec(dllexport)
#else
    #define API_EXPORT __attribute__((dllexport))
#endif

// Function Metadata
#define FUNCTION_  __FUNCTION__

#if defined(__GNUC__) || defined(__clang__)
    #define FUNCSIG_   __PRETTY_FUNCTION__
#elif defined(_MSC_VER)
    #define FUNCSIG_   __FUNCSIG__
#else
    #define FUNCSIG_   __FUNCTION__
#endif

// Inline & Restrict
#if defined(_MSC_VER)
    #define inline_    __forceinline
    #define restrict_  __restrict
    #define noalias_   __declspec(noalias)
    #define align_(x)  __declspec(align(x))
    #define format__
    #define vsnprintf  _vsnprintf
#else
    #define inline_    __attribute__((always_inline)) inline
    #define restrict_  __restrict__
    #define noalias_   
    #define align_(x)  __attribute__((aligned(x)))
    #define format__   __attribute__((format(printf, 2, 3)))
#endif

// ............................................................................

#endif // ~ KALI_PLATFORM_INCLUDED
