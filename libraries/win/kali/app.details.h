#ifndef KALI_APP_DETAILS_INCLUDED
#define KALI_APP_DETAILS_INCLUDED

#include <windows.h>
#include "kali/window.h"
#include "kali/ui/native.h"
#include "kali/graphics.h"
#include "kali/graphics.opengl.h" // [FIX] Completa el tipo BufferedContext

namespace kali {
namespace details {

    // [C4 FIX] Dispatcher de mensajes corregido
    template <typename T>
    struct Dispatch <T, false> {
        static LRESULT CALLBACK thunk(HWND handle, UINT msg, WPARAM w, LPARAM l) {
            T* window = Window(handle).object<T>();
            if (msg == WM_PAINT) {
                PAINTSTRUCT ps;
                HDC dc = BeginPaint(handle, &ps);
                Rect r(ps.rcPaint.left, ps.rcPaint.top, ps.rcPaint.right - ps.rcPaint.left, ps.rcPaint.bottom - ps.rcPaint.top);
                
                // Ahora BufferedContext ya no es un "incomplete type"
                graphics::BufferedContext ctx(dc, r);
                window->draw(ctx);
                
                EndPaint(handle, &ps);
                return 0;
            }
            return DefWindowProc(handle, msg, w, l);
        }
    };
}
}
#endif
