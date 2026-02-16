#ifndef GL_INCLUDED
#define GL_INCLUDED

#include <windows.h>
#include <gl/gl.h>
#include "kali/window.h"

namespace gl {
    struct Context {
        HDC dc; HGLRC rc;
        Context(HWND window, kali::Rect r) {
            dc = ::GetDC(window);
            PIXELFORMATDESCRIPTOR pfd = { sizeof(pfd), 1, PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER, PFD_TYPE_RGBA, 32 };
            int format = ::ChoosePixelFormat(dc, &pfd);
            ::SetPixelFormat(dc, format, &pfd);
            rc = ::wglCreateContext(dc);
            ::wglMakeCurrent(dc, rc);
        }
        ~Context() { ::wglMakeCurrent(0, 0); ::wglDeleteContext(rc); }
        void swap() { ::SwapBuffers(dc); }
    };

    inline void color(int argb) {
        glColor4ub((argb >> 16) & 0xFF, (argb >> 8) & 0xFF, argb & 0xFF, (argb >> 24) & 0xFF);
    }

    inline void drawRectDiagonalGradient(kali::Rect r, int c1, int c2) {
        glBegin(GL_QUADS);
        color(c1); glVertex2i(r.x, r.y);
        color(c1); glVertex2i(r.x + r.w, r.y);
        color(c2); glVertex2i(r.x + r.w, r.y + r.h);
        color(c2); glVertex2i(r.x, r.y + r.h);
        glEnd();
    }
}

namespace kali { namespace graphics { 
    /**
     * [C4 MASTER SYNC] Alias de Contexto.
     * Esto resuelve el conflicto de declaración entre gl::Context y BufferedContext.
     */
    typedef gl::Context BufferedContext; 
}}

#endif
