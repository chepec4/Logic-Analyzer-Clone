#include <cstdio>
#include <cstdlib>

#include "kali/platform.h"
#include "sp/sp.h"

using namespace sp;

inline m4f inter(const m4f& x, const m4f& y)
{
    m4f r;
    r[0] = y[0] - x[0];
    r[1] = y[2] - x[1];
    r[2] = y[1] - x[2];
    r[3] = y[2] - x[3];
    return r;
}


