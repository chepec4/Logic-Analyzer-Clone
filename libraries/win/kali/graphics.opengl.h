#ifndef GL_INCLUDED
#define GL_INCLUDED

#include <windows.h>
#include <gl/gl.h>
#include "kali/window.h"

namespace gl {

using namespace kali;

// Funciones de utilidad de renderizado (Logic Pro Style)
inline void color(int argb) {
    glColor4ub((argb >> 16) & 0xFF, (argb >> 8) & 0xFF, argb & 0xFF, (argb >> 24) & 0xFF);
}

inline void drawRectDiagonalGradient(Rect r, int c1, int c2) {
    glBegin(GL_QUADS);
    color(c1); glVertex2i(r.x, r.y);
    color(c1); glVertex2i(r.x + r.w, r.y);
    color(c2); glVertex2i(r.x + r.w, r.y + r.h);
    color(c2); glVertex2i(r.x, r.y + r.h);
    glEnd();
}

// Estructura de Contexto (Alias de BufferedContext)
struct Context {
    HDC dc;
    HGLRC rc;

    Context(HDC hdc, Rect r) {
        dc = hdc;
        PIXELFORMATDESCRIPTOR pfd = { sizeof(pfd), 1, PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER, PFD_TYPE_RGBA, 32 };
        int format = ::ChoosePixelFormat(dc, &pfd);
        ::SetPixelFormat(dc, format, &pfd);
        rc = ::wglCreateContext(dc);
        begin();
    }

    ~Context() { 
        ::wglMakeCurrent(nullptr, nullptr); 
        ::wglDeleteContext(rc); 
    }

    // Soluciona: 'struct gl::Context' has no member named 'begin'
    void begin() { 
        if (rc != ::wglGetCurrentContext()) ::wglMakeCurrent(dc, rc); 
    }
    
    void swap() { ::SwapBuffers(dc); }
};

// Soluciona: 'Font' in namespace 'gl' does not name a type
struct Font {
    Font(const char* name, bool bold, int size) { (void)name; (void)bold; (void)size; }
};

} // namespace gl

/**
 * [C4 MASTER SYNC] Unificación del espacio de nombres gráficos.
 * Resolvemos el conflicto 'conflicting declaration' mediante alias de tipos.
 */
namespace kali { namespace graphics { 
    using BufferedContext = gl::Context;
    using Font = gl::Font;
}}

#endif
