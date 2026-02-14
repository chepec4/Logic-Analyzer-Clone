# =============================================================================
# MAKEFILE MAESTRO - C4 ANALYZER
# Autor: ChepeC4 | C4 Productions
# Version: V15.0 "Flexible Regex" - Reparacion agnostica a espacios
# =============================================================================

CXX      := g++
# Template depth 3000 es seguridad, pero la reparacion logica en runtime.h es la clave.
CXXFLAGS := -O2 -m32 -msse -msse2 -msse3 -std=gnu++98 -fpermissive -fno-access-control -w \
            -Wno-narrowing -Wno-write-strings -ftemplate-depth=3000 \
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
# ZONA DE REPARACION (Regex con comodines .* para ignorar espacios)
# =============================================================================
fix-libraries:
	@echo "--- [FASE 1] Reparaciones Estructurales ---"
	-grep -q "typedef struct HINSTANCE__" ../libraries/kali/app.h || sed -i 's/typedef.*HINSTANCE__.*Module;/typedef struct HINSTANCE__ Module;/g' ../libraries/kali/app.h
	-grep -q "((char*&)" ../libraries/kali/runtime.h || sed -i 's/\*dst++= \*src++;/*((char*\&)dst)++ = *((const char*\&)src)++;/g' ../libraries/kali/runtime.h
	-grep -q "string(\"%s\"" ../libraries/win/kali/window.h || sed -i 's/!comments ? text/!comments ? string("%s", text)/g' ../libraries/win/kali/window.h
	-grep -q "//#define DBG" ../libraries/kali/dbgutils.h || sed -i 's/#define DBG (defined(_DEBUG))/\/\/#define DBG (defined(_DEBUG))/g' ../libraries/kali/dbgutils.h
	
	@echo "--- [FASE 2] Templates y Typenames ---"
	-grep -q "typedef typename T::Type V;" ../libraries/sp/coefficients.h || sed -i 's/typedef.*T::Type.*V;/typedef typename T::Type V;/g' ../libraries/sp/coefficients.h
	-grep -q "typedef typename T::Type Widget;" ../libraries/win/kali/ui/native/widgets.h || sed -i 's/typedef.*T::Type.*Widget;/typedef typename T::Type Widget;/g' ../libraries/win/kali/ui/native/widgets.h
	-grep -q "typename Widget::Handle handle" ../libraries/win/kali/ui/native/widgets.h || sed -i 's/Widget::Handle.*handle/typename Widget::Handle handle/g' ../libraries/win/kali/ui/native/widgets.h

	@echo "--- [FASE 3] REPARACION RUNTIME.H (El Bucle Infinito) ---"
	# 1. Inyectar struct I2T (Int2Type)
	-grep -q "struct I2T" ../libraries/kali/runtime.h || sed -i '/namespace kali {/a template<int N> struct I2T { enum { v = N }; };' ../libraries/kali/runtime.h
	
	# 2. Arreglar DEFINICION de funcion recursiva (Usando .* para tolerar espacios)
	# Original: template <E I> inline_ static void _(int offset)
	-sed -i 's/template.*<E.*I>.*void.*_(int.*offset)/template <int I> inline_ static void _(I2T<I>, int offset)/g' ../libraries/kali/runtime.h
	
	# 3. Arreglar LLAMADA recursiva
	# Original: _<E(I + 1)>(offset)
	-sed -i 's/_<E(I.*+.*1)>.*(offset)/_(I2T<I + 1>(), offset)/g' ../libraries/kali/runtime.h
	
	# 4. Arreglar LLAMADA INICIAL en ctor
	# Original: _<E(0)>(
	-sed -i 's/_<E(0)>.*(/_(I2T<0>(), /g' ../libraries/kali/runtime.h
	
	# 5. Arreglar CASO BASE (Especializacion -> Sobrecarga)
	# Original: template <> static void _ <Size> (int) {}
	-sed -i 's/template.*<>.*static.*void.*_.*<.*>.*(int).*{}/static void _(I2T<N>, int) {}/g' ../libraries/kali/runtime.h

	# 6. Arreglar ARRAY CHAR (Buffer overflow fix)
	-sed -i 's/static.*char.*aux\[\].*=.*FUNCSIG_;/static const char* aux = FUNCSIG_;/g' ../libraries/kali/runtime.h
	-grep -q "// aux\[sizeof" ../libraries/kali/runtime.h || sed -i 's/aux\[sizeof/ \/\/ aux[sizeof/g' ../libraries/kali/runtime.h

	@echo "--- [FASE 4] Platform Macro ---"
	-grep -q "__PRETTY_FUNCTION__" ../libraries/win/kali/platform.h || sed -i 's/__FUNCSIG__/__PRETTY_FUNCTION__/g' ../libraries/win/kali/platform.h

	@echo "--- [FASE 5] Adelantamiento (Containers) ---"
	-grep -q "typedef typename T::iterator" ../libraries/kali/containers.h 2>/dev/null || sed -i 's/typedef.*T::iterator/typedef typename T::iterator/g' ../libraries/kali/containers.h
	-grep -q "typedef typename T::Value" ../libraries/kali/containers.h 2>/dev/null || sed -i 's/typedef.*T::Value/typedef typename T::Value/g' ../libraries/kali/containers.h
	
	@echo "--- [FASE 6] App Details (Namespaces & Templates) ---"
	-grep -q "typedef typename T::" ../libraries/win/kali/app.details.h || sed -i 's/typedef.*T::/typedef typename T::/g' ../libraries/win/kali/app.details.h
	-sed -i 's/Loaded<T>>/Loaded<T> >/g' ../libraries/win/kali/app.details.h
	-grep -q "kali::ui::native::widget::Base::thunk_" ../libraries/win/kali/app.details.h || sed -i 's/widget::Base::thunk_/kali::ui::native::widget::Base::thunk_/g' ../libraries/win/kali/app.details.h
	-grep -q "kali::ui::native::widget::Base::drawThunk_" ../libraries/win/kali/app.details.h || sed -i 's/widget::Base::drawThunk_/kali::ui::native::widget::Base::drawThunk_/g' ../libraries/win/kali/app.details.h

	@echo "--- Compilando C4 Analyzer... ---"

# =============================================================================
# COMPILACION
# =============================================================================

$(TARGET): $(OBJS)
	@echo "--- ENLAZANDO DLL FINAL ---"
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
