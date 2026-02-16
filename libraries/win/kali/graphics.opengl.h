#ifndef GL_INCLUDED
#define GL_INCLUDED

#include <malloc.h>
#include <windows.h>
#include <gl/gl.h>

#include "kali/dbgutils.h"
#include "sp/curves.h"

namespace gl {

using namespace kali;

inline void error(const char* id = "") {
    // Stub de error
}

// [C4 FIX] Helper necesario para inicializar OpenGL en Windows
inline void setPixelFormat(HDC dc)
{
    PIXELFORMATDESCRIPTOR pfd = {
        sizeof(PIXELFORMATDESCRIPTOR), 1,
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
        PFD_TYPE_RGBA, 32, 
        0, 0, 0, 0, 0, 0, 
        0, 0, 
        0, 0, 0, 0, 0,
        16, // Depth buffer
        0, 0, PFD_MAIN_PLANE, 0, 0, 0, 0
    };
    int format = ::ChoosePixelFormat(dc, &pfd);
    ::SetPixelFormat(dc, format, &pfd);
}

struct Context
{
    HDC   dc;
    HGLRC rc;

    Context(HWND window)
    {
        dc = ::GetDC(window);
        setPixelFormat(dc);
        rc = ::wglCreateContext(dc);
        
        // Activar contexto inmediatamente para configuración
        if (rc) ::wglMakeCurrent(dc, rc);
        
        // Configuración inicial
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        size(window);
    }

    ~Context()
    {
        ::wglMakeCurrent(nullptr, nullptr);
        ::wglDeleteContext(rc);
        // ReleaseDC se debería llamar idealmente, pero el dtor de Window destruye el HWND
    }

    void size(HWND window)
    {
        RECT r; ::GetClientRect(window, &r);
        int w = r.right;
        int h = r.bottom;
        glViewport(0, 0, w, h);
        
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0, w, h, 0, -1, 1); // Coordenadas 2D (0,0 arriba-izq)
        
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
    }

    void begin()
    {
        if (rc != ::wglGetCurrentContext())
            ::wglMakeCurrent(dc, rc);
    }

    void end()
    {
        // No-op
    }
    
    void swap()
    {
        ::SwapBuffers(dc);
    }
};

// Font stub simple para evitar dependencias complejas
struct Font {
    Font(const char*, bool, int) {}
};

} // namespace gl

// [C4 FIX] Typedef para compatibilidad con app.details.h
namespace kali { namespace graphics { 
    typedef gl::Context BufferedContext; 
    typedef gl::Font Font;
}}

#endif
