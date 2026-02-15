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
        if (value < 0) --value;
        mark_(value, true);

        int align = TPM_RIGHTALIGN | TPM_TOPALIGN;
        POINT p = {x, y};
        ::ClientToScreen(window->handle, &p);
        int v = ::TrackPopupMenu(handle,
            TPM_NONOTIFY | TPM_RETURNCMD | TPM_LEFTBUTTON |
            align, p.x, p.y, 0, window->handle, 0);

        if (v) callback(v - 1);
        mark_(value, false);
    }

    Menu(const Menu& m) : handle(::CreatePopupMenu())
    {
        string s;
        int n = ::GetMenuItemCount(m.handle) + 1;
        for (int i = 1; i < n; i++)
        {
            ::GetMenuString(m.handle, i, s(), s.size, MF_BYCOMMAND);
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
        if (!aux) aux = app->autorelease(new Font);
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

namespace widget {

struct Base : Interface
{
    typedef Window::Handle Handle;
    Handle handle;

    bool enable() const  {return !!::IsWindowEnabled(handle);}
    void enable(bool v)  {::EnableWindow(handle, v);}
    bool visible() const {return !!::IsWindowVisible(handle);}
    void visible(bool v) {::ShowWindow(handle, SW_SHOW * v);}
    int  value() const   {return 0;}
    void value(int)      {}
    int  range() const   {return 1;}
    void range(int)      {}

    void text(const char* v) {::SetWindowText(handle, v);}
    string text() const {
        string v;
        ::GetWindowText(handle, v(), v.size);
        return v;
    }

    Point position() {
        RECT r; ::GetWindowRect(handle, &r);
        ::MapWindowPoints(0, ::GetParent(handle), (POINT*)&r, 2);
        return Point(r.left, r.top);
    }

    void size(int w, int h) {
        ::SetWindowPos(handle, 0, 0, 0, w, h, SWP_NOMOVE | SWP_NOZORDER | SWP_NOREDRAW);
    }

    Size size() {
        RECT r; ::GetClientRect(handle, &r);
        return Size(r.right, r.bottom);
    }

    virtual void ctor(Parent* /*parent*/, Handle h) {
        handle = h;
        Window(handle).object(this);
    }

    static void thunk_(Handle src, int flags) {
        Base* widget = Window(src).object<Base>();
        if (widget) widget->action(flags);
    }

    static void drawThunk_(DRAWITEMSTRUCT* ds) {
        Base* widget = Window(ds->hwndItem).object<Base>();
        if (widget) widget->draw(ds);
    }

    Window::Handle expose() const {return handle;};

    enum { style_ = 0, styleEx_ = 0, styleRC_ = 0, styleMaskRC_ = 0 };

    Base() : handle(0) {}

protected:
    int msg(UINT m, WPARAM w, LPARAM l) const { return (int)::SendMessage(handle, m, w, l); }
    virtual void action(int) {callback(value());}
    virtual void draw(DRAWITEMSTRUCT*) {}
};

// ............................................................................

struct Text : Base {
    static const char* class_() {return WC_STATIC;}
    enum {style_ = SS_CENTERIMAGE};
};

struct TextRight : Text {
    enum {style_ = SS_RIGHT | SS_CENTERIMAGE};
};

struct Edit : Base {
    static const char* class_() {return WC_EDIT;}
    enum { style_ = ES_AUTOHSCROLL | WS_TABSTOP, styleEx_ = WS_EX_CLIENTEDGE };
protected:
    void action(int flags) { (flags == EN_KILLFOCUS) ? extCallback(value()) : callback(0); }
};

struct TextCopy : Edit {
    enum { style_ = ES_READONLY | ES_MULTILINE | ES_AUTOVSCROLL | WS_TABSTOP };
};

struct Button : Base {
    static const char* class_() {return WC_BUTTON;}
    enum { styleRC_ = BS_PUSHBUTTON, styleMaskRC_ = BS_TYPEMASK, style_ = styleRC_ | WS_TABSTOP };
};

struct Toggle : Button {
    enum { styleRC_ = BS_AUTOCHECKBOX, style_ = styleRC_ | WS_TABSTOP };
    int  value() const {return msg(BM_GETCHECK, 0, 0);}
    void value(int v)  {msg(BM_SETCHECK, v, 0);}
};

struct Fader : Base {
    static const char* class_() {return TRACKBAR_CLASS;}
    int  value() const {return msg(TBM_GETPOS, 0, 0);}
    void value(int v)  {msg(TBM_SETPOS, TRUE, v);}
};

struct Stepper : Base {
    static const char* class_() {return UPDOWN_CLASS;}
    int  value() const {return msg(UDM_GETPOS32, 0, 0);}
    void value(int v)  {msg(UDM_SETPOS32, 0, v);}
};

struct Combo : Base {
    static const char* class_() {return WC_COMBOBOX;}
    enum {style_ = CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP};
    int  value() const {return msg(CB_GETCURSEL, 0, 0);}
    void value(int v)  {msg(CB_SETCURSEL, v, 0);}
    void clear() {msg(CB_RESETCONTENT, 0, 0);}
    int  add(const char* t) {return msg(CB_ADDSTRING, 0, (LPARAM)t);}
};

struct ColorWell : Base {
    static const char* class_() {return WC_STATIC;}
    int value() const {return value_;}
    void value(int v) { value_ = v; ::InvalidateRect(handle, 0, 0); }
    static COLORREF& custom(int i) { static COLORREF c[16]; return c[i]; }
protected:
    void draw(DRAWITEMSTRUCT* ds) {
        HBRUSH b = CreateSolidBrush(value_ & 0xFFFFFF);
        FillRect(ds->hDC, &ds->rcItem, b);
        DeleteObject(b);
    }
    int value_;
};

// ............................................................................

struct LayerTabs : Base {
    static const char* class_() {return WC_TABCONTROL;}
    int add(const char* text, Window* window) {
        TCITEM ti = {TCIF_TEXT | TCIF_PARAM, 0, 0, (char*)text, 0, (LPARAM)window->handle};
        int ret = msg(TCM_INSERTITEM, msg(TCM_GETITEMCOUNT, 0, 0), (LPARAM)&ti);
        return ret;
    }
};

struct ImageList {
    HIMAGELIST handle;
    ImageList(int n, const char* rc) { handle = ImageList_Create(16, 16, ILC_COLOR32, n, 0); }
    ~ImageList() { if(handle) ImageList_Destroy(handle); }
};

struct Toolbar : Base {
    static const char* class_() {return TOOLBARCLASSNAME;}
    void add(int n, const char* off, const char* on = 0) { /* Lógica de iconos */ }
};

// ............................................................................

struct ResourceCtor
{
    struct Aux {
        HWND h; Parent* p;
        Aux(Parent* p, int tag) : p(p) { h = ::GetDlgItem(p->window()->handle, tag); }
        
        template <typename T>
        operator Ptr<T> () const {
            T* w = new T(); w->ctor(p, h);
            p->attach(w);
            return w;
        }
        
        // Evita que el compilador intente instanciar Interface pura
        operator Ptr<Interface> () const { return Ptr<Interface>(); }
    };

    Parent* p;
    ResourceCtor(Parent* p) : p(p) {}
    Aux operator() (int tag) const { return Aux(p, tag); }
};

// ............................................................................

template <typename T> inline
typename T::Type* Ctor(Parent* parent, const Rect& r, const char* text = 0)
{
    typedef typename T::Type Widget;
    HWND h = CreateWindowEx(Widget::styleEx_, Widget::class_(), text,
        WS_CHILD | WS_VISIBLE | Widget::style_,
        r.x, r.y, r.w, r.h, parent->window()->handle, 0, GetModuleHandle(0), 0);
    
    if (h) {
        Widget* w = new Widget();
        w->ctor(parent, h);
        parent->attach(w);
        return w;
    }
    return 0;
}

} // ~ namespace widget
} // ~ namespace native
} // ~ namespace ui

using ui::native::Timer;
using ui::native::WaitCursor;

} // ~ namespace kali

#endif
