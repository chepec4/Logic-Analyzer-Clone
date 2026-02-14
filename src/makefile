# =============================================================================
# MAKEFILE MAESTRO - C4 ANALYZER
# Autor: ChepeC4 | C4 Productions
# Validacion: GCC 15 Legacy Patching - Version Robusta e Idempotente
# =============================================================================

CXX      := g++
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

LIBS     := -static-libgcc -static-libstdc++ \
            -lgdi32 -luser32 -lcomdlg32 -lopengl32 -lwinmm -lole32 -lshlwapi

OBJS     := vstsdk-wrapper.o sp.o main.res
TARGET   := C4Analyzer.dll

# =============================================================================
# OBJETIVO PRINCIPAL
# =============================================================================
all: fix-libraries $(TARGET)

# =============================================================================
# ZONA DE REPARACION INTELIGENTE (VERIFICA ANTES DE APLICAR)
# =============================================================================
fix-libraries:
	@echo "--- [FASE 1] Verificando y Reparando Sintaxis Basica ---"
	# 1. Kali App: Struct HINSTANCE
	-grep -q "typedef struct HINSTANCE__" ../libraries/kali/app.h || sed -i 's/typedef HINSTANCE__ Module;/typedef struct HINSTANCE__ Module;/g' ../libraries/kali/app.h
	
	# 2. Kali Runtime: Aritmetica void*
	-grep -q "((char*&)" ../libraries/kali/runtime.h || sed -i 's/\*dst++= \*src++;/*((char*\&)dst)++ = *((const char*\&)src)++;/g' ../libraries/kali/runtime.h
	
	# 3. Kali Window: String printf
	-grep -q "string(\"%s\"" ../libraries/win/kali/window.h || sed -i 's/!comments ? text/!comments ? string("%s", text)/g' ../libraries/win/kali/window.h
	
	# 4. Kali DbgUtils: Macro DBG
	-grep -q "//#define DBG" ../libraries/kali/dbgutils.h || sed -i 's/#define DBG (defined(_DEBUG))/\/\/#define DBG (defined(_DEBUG))/g' ../libraries/kali/dbgutils.h
	
	# 5. SP Coefficients: Typename en template
	-grep -q "typedef typename T::Type V;" ../libraries/sp/coefficients.h || sed -i 's/typedef T::Type V;/typedef typename T::Type V;/g' ../libraries/sp/coefficients.h

	@echo "--- [FASE 2] Reparando Widgets (Typename faltantes) ---"
	# 6. Kali Native Widgets: typedef typename
	-grep -q "typedef typename T::Type Widget;" ../libraries/win/kali/ui/native/widgets.h || sed -i 's/typedef T::Type Widget;/typedef typename T::Type Widget;/g' ../libraries/win/kali/ui/native/widgets.h
	
	# 7. [NUEVO] Kali Native Widgets: typename en Widget::Handle (Solucion del Error Actual)
	-grep -q "typename Widget::Handle handle" ../libraries/win/kali/ui/native/widgets.h || sed -i 's/Widget::Handle handle/typename Widget::Handle handle/g' ../libraries/win/kali/ui/native/widgets.h

	@echo "--- [FASE 3] Reparando Templates en Runtime.h ---"
	# A. Estructura de ayuda I2T
	-grep -q "struct I2T" ../libraries/kali/runtime.h || sed -i '/namespace kali {/a template<int N> struct I2T { enum { v = N }; };' ../libraries/kali/runtime.h
	
	# B. Firma funcion recursiva
	-grep -q "void _(I2T" ../libraries/kali/runtime.h || sed -i 's/template <int i> static void _(int)/template <int i> static void _(I2T<i>)/g' ../libraries/kali/runtime.h
	
	# C. Llamada recursiva
	-grep -q "I2T<i+1>" ../libraries/kali/runtime.h || sed -i 's/ _<i+1>(0);/ _<i+1>(I2T<i+1>());/g' ../libraries/kali/runtime.h
	
	# D. Especializacion -> Sobrecarga
	-grep -q "static void _(I2T<Size>)" ../libraries/kali/runtime.h || sed -i 's/template <> static void _ <Size> (int) {}/static void _(I2T<Size>) {}/g' ../libraries/kali/runtime.h
	
	@echo "--- Validacion completa. Procediendo a compilar... ---"

# =============================================================================
# COMPILACION
# =============================================================================

$(TARGET): $(OBJS)
	@echo "--- ENLAZANDO DLL FINAL ---"
	$(CXX) -shared -m32 -o $@ $(OBJS) $(LIBS) -Wl,--export-all-symbols -Wl,--kill-at -Wl,--defsym=main=VSTPluginMain
	@echo "--- EXITO: Plugin C4 Analyzer Creado ---"

vstsdk-wrapper.o: vstsdk-wrapper.cpp
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

sp.o: sp.cpp
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

main.res: main.rc
	windres main.rc -O coff -o main.res

clean:
	rm -f *.o *.res *.dll

.PHONY: all fix-libraries clean
