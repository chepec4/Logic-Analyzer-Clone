# =============================================================================
# MAKEFILE MAESTRO - C4 ANALYZER
# Autor: ChepeC4 | C4 Productions
# Nivel: V9.0 "Nuclear Fix" - Reescritura de Recursion y Arrays
# =============================================================================

CXX      := g++
# Flags de compilador optimizados
CXXFLAGS := -O2 -m32 -msse -msse2 -msse3 -std=gnu++98 -fpermissive -fno-access-control -w \
            -Wno-narrowing -Wno-write-strings -ftemplate-depth=2000 \
            -D_WIN32_WINNT=0x0600 -DBUILDING_VST=1 \
            -DNAME="\"C4 Analyzer\"" \
            -DCOMPANY="\"C4 Productions\"" \
            -DAUTHOR="\"ChepeC4\""

INC      := -I. \
            -I../libraries \
            -I../libraries/win \
            -I../libraries/vst \
            -I../libraries/vst/public.sdk/source/vst2.x \
            -I../libraries/vst/pluginterfaces/vst2.x \
            -I../libraries/sp

LIBS     := -static-libgcc -static-libstdc++ \
            -lgdi32 -luser32 -lcomdlg32 -lopengl32 -lwinmm -lole32 -lshlwapi

OBJS     := vstsdk-wrapper.o sp.o main.res
TARGET   := C4Analyzer.dll

# =============================================================================
# OBJETIVO PRINCIPAL
# =============================================================================
all: fix-libraries $(TARGET)

# =============================================================================
# ZONA DE REPARACION
# =============================================================================
fix-libraries:
	@echo "--- [FASE 1] Reparaciones Estructurales ---"
	-grep -q "typedef struct HINSTANCE__" ../libraries/kali/app.h || sed -i 's/typedef HINSTANCE__ Module;/typedef struct HINSTANCE__ Module;/g' ../libraries/kali/app.h
	-grep -q "((char*&)" ../libraries/kali/runtime.h || sed -i 's/\*dst++= \*src++;/*((char*\&)dst)++ = *((const char*\&)src)++;/g' ../libraries/kali/runtime.h
	-grep -q "string(\"%s\"" ../libraries/win/kali/window.h || sed -i 's/!comments ? text/!comments ? string("%s", text)/g' ../libraries/win/kali/window.h
	-grep -q "//#define DBG" ../libraries/kali/dbgutils.h || sed -i 's/#define DBG (defined(_DEBUG))/\/\/#define DBG (defined(_DEBUG))/g' ../libraries/kali/dbgutils.h

	@echo "--- [FASE 2] Reparaciones de Templates ---"
	-grep -q "typedef typename T::Type V;" ../libraries/sp/coefficients.h || sed -i 's/typedef T::Type V;/typedef typename T::Type V;/g' ../libraries/sp/coefficients.h
	-grep -q "typedef typename T::Type Widget;" ../libraries/win/kali/ui/native/widgets.h || sed -i 's/typedef T::Type Widget;/typedef typename T::Type Widget;/g' ../libraries/win/kali/ui/native/widgets.h
	-grep -q "typename Widget::Handle handle" ../libraries/win/kali/ui/native/widgets.h || sed -i 's/Widget::Handle handle/typename Widget::Handle handle/g' ../libraries/win/kali/ui/native/widgets.h

	@echo "--- [FASE 3] Correccion de Macro Plataforma ---"
	-grep -q "__PRETTY_FUNCTION__" ../libraries/win/kali/platform.h || sed -i 's/__FUNCSIG__/__PRETTY_FUNCTION__/g' ../libraries/win/kali/platform.h

	@echo "--- [FASE 4] CIRUGIA MAYOR EN RUNTIME.H (Recursion Infinita y Array) ---"
	# 1. Inyectar struct I2T para controlar la sobrecarga
	-grep -q "struct I2T" ../libraries/kali/runtime.h || sed -i '/namespace kali {/a template<int N> struct I2T { enum { v = N }; };' ../libraries/kali/runtime.h
	
	# 2. Cambiar la firma de la funcion recursiva para usar I2T y int puro
	-sed -i 's/template <E I> inline_ static void _(int offset)/template <int I> static void _(I2T<I>, int offset)/g' ../libraries/kali/runtime.h
	
	# 3. Corregir la llamada recursiva para pasar el objeto I2T y evitar el bucle infinito
	-sed -i 's/_<E(I + 1)>(offset)/_(I2T<I + 1>(), offset)/g' ../libraries/kali/runtime.h
	
	# 4. Corregir la llamada inicial en el constructor
	-sed -i 's/_<E(0)>(/_(I2T<0>(), /g' ../libraries/kali/runtime.h
	
	# 5. Definir el CASO BASE para detener la recursion (Sobrecarga vacia)
	# Reemplazamos la especializacion antigua por una sobrecarga valida
	-sed -i 's/template <> static void _ <Size> (int) {}/static void _(I2T<N>, int) {}/g' ../libraries/kali/runtime.h

	# 6. ARREGLO DEL ARRAY CHAR (El error de inicializacion)
	# Usamos un buffer local seguro en lugar de intentar inicializar con la variable del sistema
	-sed -i 's/static char aux\[\] = FUNCSIG_;/char aux[512]; strncpy(aux, FUNCSIG_, 511); aux[511]=0;/g' ../libraries/kali/runtime.h
	
	# 7. Desactivar logica de recorte de string (Incompatible con GCC y causaba error de sizeof)
	-sed -i 's/aux\[sizeof(aux)/ \/\/ aux\[sizeof(aux)/g' ../libraries/kali/runtime.h

	@echo "--- [FASE 5] ADELANTAMIENTO (Containers y AppDetails) ---"
	-grep -q "typedef typename T::iterator" ../libraries/kali/containers.h 2>/dev/null || sed -i 's/typedef T::iterator/typedef typename T::iterator/g' ../libraries/kali/containers.h
	-grep -q "typedef typename T::Value" ../libraries/kali/containers.h 2>/dev/null || sed -i 's/typedef T::Value/typedef typename T::Value/g' ../libraries/kali/containers.h
	-grep -q "typedef typename T::" ../libraries/win/kali/app.details.h || sed -i 's/typedef T::/typedef typename T::/g' ../libraries/win/kali/app.details.h
	-sed -i 's/Loaded<T>>/Loaded<T> >/g' ../libraries/win/kali/app.details.h

	@echo "--- Codigo Blindado. Compilando... ---"

# =============================================================================
# COMPILACION
# =================================================================
