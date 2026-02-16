#ifndef KALI_TYPES_INCLUDED
#define KALI_TYPES_INCLUDED

#include "kali/platform.h"

namespace kali {

enum Align  {left, center, right};
enum UpDown {up, down};
enum Weight {regular, bold};

struct Null {};
struct Undefined;

struct ReleaseAny
{
    virtual ~ReleaseAny() {}
};

template <typename T>
struct Ptr
{
    typedef T Type;

    Ptr(Type* ptr = nullptr) : ptr(ptr) {}
    operator Type* () const {return   ptr;}
    operator Type&    () const {return  *ptr;}
    operator bool     () const {return !!ptr;}
    Type& operator * () const {return  *ptr;}
    Type* operator -> () const {return   ptr;}

private:
    Type* ptr;
    template <typename U> operator U () const;
};

template <typename T, int Unique = 0>
struct singleton
{
    typedef T Type;

    singleton() {}
    Type* operator -> () const {return   ptr;}
    operator bool ()     const {return !!ptr;}

    static Type* alloc()
    {
        ptr = new Type;
        return ptr;
    }

    static void release()
    {
        if (ptr) {
            delete ptr;
            ptr = nullptr;
        }
    }

private:
    static Type* ptr;
    template <typename U> operator U () const;
};

template <typename T, int Unique>
T* singleton<T, Unique>::ptr = nullptr;

} // ~ namespace kali

#endif // KALI_TYPES_INCLUDED
