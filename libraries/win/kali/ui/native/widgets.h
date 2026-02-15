#ifndef KALI_UI_NATIVE_WIDGETS_INCLUDED
#define KALI_UI_NATIVE_WIDGETS_INCLUDED

#include <windows.h>
#include <commctrl.h>
#include "kali/ui/native/widgets.base.h"
#include "kali/ui/native/details.h"

// ............................................................................

namespace kali   {
namespace ui     {
namespace native {

// ............................................................................

struct Menu : UsesCallback
{
    void operator += (const char* item)
    {
        ::AppendMenu(handle, MF_STRING,
            ::GetMenuItemCount(handle) + 1, item);
    }

    string text(int index) const
    {
        string s = "? ? ?";
        ::GetMenuString(handle,
            index + 1, s(), s.size, MF_BYCOMMAND);
        return s;
    }

    void show(Window* window, const Point& p, int value = ~0)
    {
        show(window, p.x, p.y, value);
    }

    void show(Window* window, int x, int y, int value = ~0)
    {
        if (value < 0)
            --value;
        mark_(value, true);

        int align = TPM_RIGHTALIGN | TPM_TOPALIGN;

        POINT p = {x, y};
        ::ClientToScreen(window->handle, &p);
        int v = ::TrackPopupMenu(handle,
            TPM_NONOTIFY | TPM_RETURNCMD | TPM_LEFTBUTTON |
            align, p.x, p.y, 0, window->handle, 0);

        if (v)
            callback(v - 1);

        mark_(value, false);
    }

    Menu(const Menu& m) : handle(::CreatePopupMenu())
    {
        string s;
        int n = ::GetMenuItemCount(m.handle) + 1;
        for (int i = 1; i < n; i++)
        {
            ::GetMenuString(m.handle,
                i, s(), s.size, MF_BYCOMMAND);
            ::AppendMenu(handle, MF_STRING, i, s);
        }
    }

    Menu() : handle(::CreatePopupMenu()) {}
    ~Menu() {::DestroyMenu(handle);}

private:

    void mark_(int index, bool m)
    {
        ::CheckMenuItem(handle, index + 1, MF_BYCOMMAND
            | (m ? MF_CHECKED : MF_UNCHECKED));
    }

private:
    HMENU handle;
};

// ............................................................................

struct Font : ReleaseAny
{
    struct Scale
    {
        enum {refx = 6, refy = 13};
        int x(int v) {return (v * x_ + refx/2) / refx;}
        int y(int v) {return (v * y_) / refy;}
        Scale(int x_, int y_) : x_(x_), y_(y_) {}

    private:
        int x_, y_;
    };

    static const Font& main()
    {
        static Font* aux = 0;
        if (!aux)
            aux = app->autorelease(new Font);
        return *aux;
    }

    Scale    scale() const {return scale(handle);}
    operator HFONT() const {return handle;}

    Font() : handle(ctor()) {}
    ~Font() {::DeleteObject(handle);}

private:
    HFONT handle;

    Font(const Font&);
    Font& operator = (const Font&);
    template <typename T> operator T () const;

private:

    static HFONT ctor()
    {
        NONCLIENTMETRICS ncm = {sizeof(ncm)};
        ::SystemParametersInfo(SPI_GETNONCLIENTMETRICS, 0, &ncm, 0);
        return ::CreateFontIndirect(&ncm.lfMessageFont);
    }

    static Scale scale(HFONT handle)
    {
        HDC dc  = ::CreateCompatibleDC(0);
        HGDIOBJ f = ::SelectObject(dc, handle);

        TEXTMETRIC tm;
        ::GetTextMetrics(dc, &tm);
        int y = tm.tmHeight;

        const char a[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
        const int n = sizeof(a) - 1;
        SIZE size;
        ::GetTextExtentPoint32(dc, a, n, &size);
        int x = (size.cx + (n / 2)) / n;

        ::SelectObject(dc, f);
        ::DeleteDC(dc);

        return Scale(x, y);
    }
};

// ............................................................................

template <typename T, typename R>
inline void EnumerateFonts(T* obj, R (T::*func)(const char*))
{
    struct Func
    {
        typedef R (T::*F)(const char*);
        Func(T* obj, F func) : obj(obj), func(func) {}
        T* obj;
        F func;

        static int CALLBACK thunk(const LOGFONT* lf,
            const TEXTMETRIC* /*tm*/, DWORD /*type*/, LPARAM ptr)
        {
            Func* func = (Func*) ptr;
            (func->obj->*func->func)
                (lf->lfFaceName);
            return ~0;
        }
    };

    HDC dc = ::GetDC(0);
    Func f(obj, func);
    ::EnumFontFamilies(dc, 0, &Func::thunk, (LPARAM) &f);
    ::ReleaseDC(0, dc);
}

// ............................................................................

struct TooltipSupport
{
    TooltipSupport() : handle(0) {}
    ~TooltipSupport() {clear(0);}

    void attach(Window* window, const Rect& r, const char* text)
    {
        if (!this || r.empty())
            return;

        if (!handle)
            handle = ctor(window);

        TOOLINFO ti;
        RECT rect   = {r.x, r.y, r.right(), r.bottom()};
        ti.cbSize   = sizeof(TOOLINFO);
        ti.uFlags   = TTF_SUBCLASS;
        ti.hwnd     = window->handle;
        ti.uId      = 1;
        ti.rect     = rect;
        ti.hinst    = 0;
        ti.lpszText = (char*) (text);
        ti.lParam   = 0;

        if (!::SendMessage(handle, TTM_ADDTOOL, 0, (LPARAM) &ti))
            trace("%s: TTM_ADDTOOL failed [%i]\n", FUNCTION_, ::GetLastError());
    }

    void clear(Window*)
    {
        ::DestroyWindow(handle);
        handle = 0;
    }

private:

    static HWND ctor(Window* window)
    {
        HWND handle = ::CreateWindowEx(WS_EX_TOPMOST,
            TOOLTIPS_CLASS, 0, WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,
            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
            window->handle, 0, app->module(), 0);

        if (!handle)
            trace("%s: CreateWindowEx failed [%i]\n", FUNCTION_, ::GetLastError());

        return handle;
    }

private:
    HWND handle;
};

// ............................................................................

struct Timer : UsesCallback
{
    void start(Window* window, unsigned time)
    {
        stop();
        window_ = window;
        ::SetTimer(window_->handle, (UINT_PTR) this, time, thunk);
    }

    void stop()
    {
        if (window_)
            ::KillTimer(window_->handle, (UINT_PTR) this);
        window_ = 0;
        counter = 0;
    }

    Timer() : window_(0) {}
    ~Timer() {stop();}

private:

    static VOID CALLBACK thunk(HWND, UINT, UINT_PTR ptr, DWORD)
    {
        Timer* timer = (Timer*) ptr;
        if (timer)
            timer->callback(++timer->counter);
    }

private:
    Window* window_;
    int     counter;
};

// ............................................................................

struct WaitCursor
{
    WaitCursor()  {::SetCursor(::LoadCursor(0, IDC_WAIT));}
    ~WaitCursor() {::SetCursor(::LoadCursor(0, IDC_ARROW));}

    explicit WaitCursor(bool enable)
    {
        if (enable)
            ::SetCursor(::LoadCursor(0, IDC_WAIT));
    }
};

// ............................................................................

namespace widget {

// ............................................................................

struct Base : Interface
{
    typedef Window::Handle Handle;

    bool enable() const  {return !!::IsWindowEnabled(handle);}
    void enable(bool v)  {::EnableWindow(handle, v);}
    bool visible() const {return !!::IsWindowVisible(handle);}
    void visible(bool v) {::ShowWindow(handle, SW_SHOW * v);}
    int  value() const   {return 0;}
    void value(int)      {}
    int  range() const   {return 1;}
    void range(int)      {}

    void text(const char* v) {::SetWindowText(handle, v);}

    string text() const
    {
        string v;
        ::GetWindowText(handle, v(), v.size);
        return v;
    }

    Point position()
    {
        RECT r;
        ::GetWindowRect(handle, &r);
        ::MapWindowPoints(0,
            ::GetParent(handle), (POINT*) &r, 2);
        return Point(r.left, r.top);
    }

    void position(int x, int y)
    {
        ::SetWindowPos(handle, 0, x, y, 0, 0,
            SWP_NOSIZE | SWP_NOZORDER | SWP_NOREDRAW);
    }

    Size size()
    {
        RECT r;
        ::GetClientRect(handle, &r);
        return Size(r.right, r.bottom);
    }

    void size(int w, int h)
    {
        ::SetWindowPos(handle, 0, 0, 0, w, h,
            SWP_NOMOVE | SWP_NOZORDER | SWP_NOREDRAW);
    }

    virtual void ctor(Parent* /*parent*/, Handle h)
    {
        handle = h;
        Window(handle).object(this);
    }

    static void thunk_(Handle src, int flags)
    {
        Base* widget = object(src);
        if (widget)
            widget->action(flags);
    }

    static void drawThunk_(DRAWITEMSTRUCT* ds)
    {
        Base* widget = object(ds->hwndItem);
        if (widget)
            widget->draw(ds);
    }

    Window::Handle expose() const {return handle;};

    enum
    {
        // for indirect Ctor:
        style_       = 0,
        styleEx_     = 0,
        // for resource Ctor:
        styleRC_     = 0,
        styleMaskRC_ = 0
    };

    Base() : handle(0) {}

protected:

    int msg(UINT m, WPARAM w, LPARAM l) const
    {
        return (int) ::SendMessage(handle, m, w, l);
    }

    static Base* object(Handle handle)
    {
        return Window(handle).object<Base>();
    }

    virtual void action(int) {callback(value());}

    virtual void draw(DRAWITEMSTRUCT*) {}

    void changeStyle(int index, int remove, int add, bool recreate = false)
    {
        if (recreate)
            return ctor(0, details::cloneWindow
                (handle, remove, add, true));

        int v = ::GetWindowLong(handle, index);
        v = (~remove & v) | add;
        ::SetWindowLong(handle, index, v);
        ::SetWindowPos(handle, 0, 0, 0, 0, 0,
            SWP_NOSIZE | SWP_NOMOVE | SWP_FRAMECHANGED);
    }

public:
    Handle handle;
    Base(const Base&);
    Base& operator = (const Base&);
};

// ............................................................................

struct Null : Interface
{
    bool enable() const    {return 0;}
    void enable(bool)      {}
    bool visible() const   {return 0;}
    void visible(bool)     {}
    int  value() const     {return 0;}
    void value(int)        {}
    int  range() const     {return 0;}
    void range(int)        {}
    string text() const    {return string("%s", "");}
    void text(const char*) {}

    Window::Handle expose() const {return 0;};
};

// ............................................................................

template <typename T>
typename T::Type* Ctor(Parent*, const Rect&, const char* text = 0);

// ............................................................................

struct Text : Base
{
    static const char* class_() {return WC_STATIC;}
    enum {style_ = SS_CENTERIMAGE};
};

struct TextRight : Text
{
    enum {style_ = SS_RIGHT | SS_CENTERIMAGE};
};

struct Break : Text
{
    enum {style_ = SS_ETCHEDHORZ};
};

// ............................................................................

struct Edit : Base
{
    static const char* class_() {return WC_EDIT;}
    enum
    {
        style_   = ES_AUTOHSCROLL | WS_TABSTOP,
        styleEx_ = WS_EX_CLIENTEDGE
    };

    void append(const char* v)
    {
        msg(EM_SETSEL, (WPARAM) -1, -1);
        msg(EM_REPLACESEL, 0, (LPARAM) v);
    }

    void border(bool v)
    {
        changeStyle(GWL_EXSTYLE,
            WS_EX_CLIENTEDGE, WS_EX_CLIENTEDGE * v);
    }

protected:
    void action(int flags)
    {
        (flags == EN_KILLFOCUS)
            ? extCallback(value())
            : callback(0);
    }
};

// ............................................................................

struct Button : Base
{
    static const char* class_() {return WC_BUTTON;}
    enum
    {
        styleRC_     = BS_PUSHBUTTON,
        styleMaskRC_ = BS_TYPEMASK,
        style_       = styleRC_ | WS_TABSTOP
    };
};

struct Toggle : Button
{
    enum
    {
        styleRC_ = BS_AUTOCHECKBOX,
        style_   = styleRC_ | WS_TABSTOP
    };
    int  value() const {return msg(BM_GETCHECK, 0, 0);}
    void value(int v)  {msg(BM_SETCHECK, v, 0);}
};

// ............................................................................

struct Meter : Base
{
    static const char* class_() {return PROGRESS_CLASS;}
    int  value() const {return msg(PBM_GETPOS, 0, 0);}
    void value(int v)  {msg(PBM_SETPOS, v, 0);}
    int  range() const {return msg(PBM_GETRANGE, 0, 0);}
    void range(int v)  {msg(PBM_SETRANGE32, 0, v);}
};

struct Combo : Base
{
    static const char* class_() {return WC_COMBOBOX;}
    enum {style_ = CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP};
    int  value() const {return msg(CB_GETCURSEL, 0, 0);}
    void value(int v)  {msg(CB_SETCURSEL, v, 0);}
    int  range() const {return msg(CB_GETCOUNT, 0, 0);}
    void range(int)    {}
    void clear()       {msg(CB_RESETCONTENT, 0, 0);}
    int  add(const char* text) {return msg(CB_ADDSTRING, 0, (LPARAM) text);}
};

// ............................................................................

struct LayerTabs : Base
{
    static const char* class_() {return WC_TABCONTROL;}
    enum {style_ = WS_TABSTOP};
    
    int  value() const {return msg(TCM_GETCURSEL, 0, 0);}
    void value(int v)  {msg(TCM_SETCURSEL, v, 0); selectWindow();}
    int  range() const {return msg(TCM_GETITEMCOUNT, 0, 0);}

    int add(const char* text, Window* window)
    {
        TCITEM ti  = {0};
        ti.mask    = TCIF_TEXT | TCIF_PARAM;
        ti.pszText = (char*) text;
        ti.lParam  = (LPARAM) window->handle;
        int ret = msg(TCM_INSERTITEM, range(), (LPARAM) &ti);
        alignWindow(window);
        ::ShowWindow(window->handle, ret ? SW_HIDE : SW_SHOW);
        return ret;
    }

protected:
    void action(int) { selectWindow(); callback(value()); }
    void selectWindow() const {
        for (int i = 0; i < range(); i++) {
            TCITEM ti = {0}; ti.mask = TCIF_PARAM;
            ::SendMessage(handle, TCM_GETITEM, i, (LPARAM)&ti);
            ::ShowWindow((HWND)ti.lParam, (i == value()) ? SW_SHOW : SW_HIDE);
        }
    }
    void alignWindow(Window* window) {
        RECT r = {0, 0, 0, 0};
        msg(TCM_ADJUSTRECT, FALSE, (LPARAM) &r);
        Point p = position();
        window->position(p.x + r.left, p.y + r.top);
    }
};

// ............................................................................

struct Toolbar : Base
{
    static const char* class_() {return TOOLBARCLASSNAME;}
    void ctor(Parent* parent, Handle h) {
        Base::ctor(parent, h);
        msg(TB_BUTTONSTRUCTSIZE, sizeof(TBBUTTON), 0);
    }
protected:
    void action(int index) {callback(index);}
};

// ............................................................................

struct ResourceCtor
{
private:
    struct Aux
    {
        Aux(Parent* parent, int tag) : parent(parent), tag(tag)
        {
            handle = ::GetDlgItem(parent->window()->handle, tag);
            *class_ = 0;
            ::GetClassName(handle, class_, sizeof(class_));
            style = ::GetWindowLong(handle, GWL_STYLE);
        }

        template <typename T>
        operator Ptr<T> () const
        {
            T* widget = new T();
            widget->ctor(parent, handle);
            parent->attach(widget);
            return widget;
        }

    private:
        Base::Handle handle;
        Parent* parent;
        int          tag;
        int          style;
        char         class_[64];
    };

public:
    explicit ResourceCtor(Parent* parent) : parent(parent) {}
    Aux operator() (int tag) const {return Aux(parent, tag);}

private:
    Parent* parent;
};

// ............................................................................

// [C4 FIX] Implementaciones finales con typename corregido
template <typename T> inline
typename T::Type* Ctor(Parent* parent, const Rect& r, const char* text)
{
    typedef typename T::Type Widget;
    typename Widget::Handle handle = CreateWindowEx
       (Widget::styleEx_, Widget::class_(), text,
        WS_CHILD | WS_VISIBLE | Widget::style_,
        r.x, r.y, r.w, r.h, parent->window()->handle,
        0, ::GetModuleHandle(0), 0);

    if (handle)
    {
        ::SendMessage(handle, WM_SETFONT, (WPARAM) (HFONT) Font::main(), 0);
        Widget* widget = new Widget();
        widget->ctor(parent, handle);
        parent->attach(widget);
        return widget;
    }
    return 0;
}

} // ~ namespace widget
} // ~ namespace native
} // ~ namespace ui

// ............................................................................

using ui::native::Timer;
using ui::native::WaitCursor;

// ............................................................................

} // ~ namespace kali

#endif // ~ KALI_UI_NATIVE_WIDGETS_INCLUDED
