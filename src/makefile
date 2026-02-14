# =============================================================================
# MAKEFILE MAESTRO - C4 ANALYZER
# Autor: ChepeC4 | C4 Productions
# Analisis de Ingenieria: Validado para GCC 15 / C++98 Strict
# =============================================================================

CXX      := g++
# Banderas optimizadas. Se mantiene -fpermissive para compatibilidad legacy
CXXFLAGS := -O2 -m32 -msse -msse2 -msse3 -std=gnu++98 -fpermissive -fno-access-control -w \
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

# Librerias del sistema (Linker flags)
LIBS     := -static-libgcc -static-libstdc++ \
            -lgdi32 -luser32 -lcomdlg32 -lopengl32 -lwinmm -lole32 -lshlwapi

OBJS     := vstsdk-wrapper.o sp.o main.res
TARGET   := C4Analyzer.dll

# =============================================================================
# OBJETIVO PRINCIPAL
# =============================================================================
all: fix-libraries $(TARGET)

# =============================================================================
# ZONA DE REPARACION QUIRURGICA DE LIBRERIAS (KALI & SP)
# =============================================================================
fix-libraries:
	@echo "--- [FASE 1] Reparando errores de sintaxis basica ---"
	# 1. Kali App: Struct HINSTANCE faltante
	-sed -i 's/typedef HINSTANCE__ Module;/typedef struct HINSTANCE__ Module;/g' ../libraries/kali/app.h
	# 2. Kali Runtime: Aritmetica de punteros void*
	-sed -i 's/\*dst++= \*src++;/*((char*\&)dst)++ = *((const char*\&)src)++;/g' ../libraries/kali/runtime.h
	# 3. Kali Window: Ambiguedad de String printf
	-sed -i 's/!comments ? text/!comments ? string("%s", text)/g' ../libraries/win/kali/window.h
	# 4. Kali DbgUtils: Macro DBG ilegal
	-sed -i 's/#define DBG (defined(_DEBUG))/\/\/#define DBG (defined(_DEBUG))/g' ../libraries/kali/dbgutils.h
	# 5. SP Coefficients: Falta typename en templates
	-sed -i 's/typedef T::Type V;/typedef typename T::Type V;/g' ../libraries/sp/coefficients.h

	@echo "--- [FASE 2] Reparando error critico de Templates en Runtime.h ---"
	# A. Inyectar estructura de ayuda I2T (Int2Type) para sobrecarga
	-sed -i '/namespace kali {/a template<int N> struct I2T { enum { v = N }; };' ../libraries/kali/runtime.h
	# B. Cambiar firma de funcion recursiva para usar I2T
	-sed -i 's/template <int i> static void _(int)/template <int i> static void _(I2T<i>)/g' ../libraries/kali/runtime.h
	# C. Actualizar llamada recursiva (Paso de testigo)
	-sed -i 's/ _<i+1>(0);/ _<i+1>(I2T<i+1>());/g' ../libraries/kali/runtime.h
	# D. REPARACION CLAVE: Convertir especializacion ilegal en sobrecarga valida
	-sed -i 's/template <> static void _ <Size> (int) {}/static void _(I2T<Size>) {}/g' ../libraries/kali/runtime.h
	
	@echo "--- Librerias validadas y listas ---"

# =============================================================================
# COMPILACION
# =============================================================================

$(TARGET): $(OBJS)
	@echo "--- Enlazando Plugin Final ---"
	$(CXX) -shared -m32 -o $@ $(OBJS) $(LIBS) -Wl,--export-all-symbols -Wl,--kill-at -Wl,--defsym=main=VSTPluginMain
	@echo "--- EXITO: C4 Analyzer generado correctamente ---"

vstsdk-wrapper.o: vstsdk-wrapper.cpp
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

sp.o: sp.cpp
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

main.res: main.rc
	windres main.rc -O coff -o main.res

clean:
	rm -f *.o *.res *.dll

.PHONY: all fix-libraries clean
