#include <cstdio>
#include <cstdlib>

#include "kali/platform.h"
#include "sp/sp.h"

using namespace sp;

// -----------------------------------------------------------------------------
// Interpolación simple entre dos vectores m4f
// (reemplazo portable del código SSE antiguo)
// -----------------------------------------------------------------------------

inline m4f inter(const m4f& x, const m4f& y)
{
    m4f r;
    r.v[0] = y.v[0] - x.v[0];
    r.v[1] = y.v[2] - x.v[1];
    r.v[2] = y.v[1] - x.v[2];
    r.v[3] = y.v[2] - x.v[3];
    return r;
}

// -----------------------------------------------------------------------------
// Test simple de funcionamiento
// -----------------------------------------------------------------------------

void usage()
{
    m4f y = {{0.f, 1.f, 2.f, 3.f}};
    m4f x = {{4.f, 5.f, 6.f, 7.f}};

    m4f z = inter(x, y);

    printf("Resultado:\n");
    for (int i = 0; i < 4; ++i)
        printf("z[%d] = %f\n", i, z.v[i]);
}

// -----------------------------------------------------------------------------
// main (solo test)
// -----------------------------------------------------------------------------

int main()
{
    usage();
    return 0;
}
