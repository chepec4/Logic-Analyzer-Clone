#ifndef GL_INCLUDED
#define GL_INCLUDED

#include <windows.h>
#include <gl/gl.h>
#include "kali/window.h"

namespace gl {

struct Context {
    HDC dc;
    HGLRC rc;
    Context(HDC hdc, kali::Rect r) {
        dc = hdc;
        PIXELFORMATDESCRIPTOR pfd = { sizeof(pfd), 1, PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER, PFD_TYPE_RGBA, 32 };
        int format = ::ChoosePixelFormat(dc, &pfd);
        ::SetPixelFormat(dc, format, &pfd);
        rc = ::wglCreateContext(dc);
        ::wglMakeCurrent(dc, rc);
    }
    ~Context() { ::wglMakeCurrent(nullptr, nullptr); ::wglDeleteContext(rc); }
    void swap() { ::SwapBuffers(dc); }
};

} // namespace gl

/**
 * [C4 MASTER SYNC] Tipo Unificado de Contexto Gráfico
 * Resuelve: error: conflicting declaration 'typedef struct gl::Context kali::graphics::BufferedContext'
 */
namespace kali { namespace graphics { 
    struct BufferedContext : gl::Context {
        using gl::Context::Context;
    };
}}

#endif
