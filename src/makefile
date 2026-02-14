# =============================================================================
# MAKEFILE BOOTSTRAPPER V11.0 (Generador Automatico)
# Autor: ChepeC4 - Correccion de Namespaces y Templates
# =============================================================================

all:
	@echo "--- Generando Makefile corregido (V11.0) ---"
	@echo "CXX := g++" > Makefile.run
	@echo "CXXFLAGS := -O2 -m32 -msse -msse2 -msse3 -std=gnu++98 -fpermissive -fno-access-control -w -Wno-narrowing -Wno-write-strings -ftemplate-depth=3000 -D_WIN32_WINNT=0x0600 -DBUILDING_VST=1 -DNAME=\"\\\"C4 Analyzer\\\"\" -DCOMPANY=\"\\\"C4 Productions\\\"\" -DAUTHOR=\"\\\"ChepeC4\\\"\"" >> Makefile.run
	@echo "INC := -I. -I../libraries -I../libraries/win -I../libraries/vst -I../libraries/vst/public.sdk/source/vst2.x -I../libraries/vst/pluginterfaces/vst2.x -I../libraries/sp" >> Makefile.run
	@echo "LIBS := -static-libgcc -static-libstdc++ -lgdi32 -luser32 -lcomdlg32 -lopengl32 -lwinmm -lole32 -lshlwapi" >> Makefile.run
	@echo "OBJS := vstsdk-wrapper.o sp.o main.res" >> Makefile.run
	@echo "TARGET := C4Analyzer.dll" >> Makefile.run
	@echo "VPATH := ../libraries/sp" >> Makefile.run
	@echo "" >> Makefile.run
	@echo "all: fix-libraries \$$(TARGET)" >> Makefile.run
	@echo "" >> Makefile.run
	@echo "fix-libraries:" >> Makefile.run
	@echo "	@echo \"--- [FASE 1] Reparaciones Estructurales ---\"" >> Makefile.run
	@echo "	-grep -q \"typedef struct HINSTANCE__\" ../libraries/kali/app.h || sed -i 's/typedef HINSTANCE__ Module;/typedef struct HINSTANCE__ Module;/g' ../libraries/kali/app.h" >> Makefile.run
	@echo "	-grep -q \"((char*&)\" ../libraries/kali/runtime.h || sed -i 's/\*dst++= \*src++;/*((char*\\&)dst)++ = *((const char*\\&)src)++;/g' ../libraries/kali/runtime.h" >> Makefile.run
	@echo "	-grep -q \"string(\\\"%s\\\"\" ../libraries/win/kali/window.h || sed -i 's/!comments ? text/!comments ? string(\"%s\", text)/g' ../libraries/win/kali/window.h" >> Makefile.run
	@echo "	-grep -q \"//#define DBG\" ../libraries/kali/dbgutils.h || sed -i 's/#define DBG (defined(_DEBUG))/\\/\\/#define DBG (defined(_DEBUG))/g' ../libraries/kali/dbgutils.h" >> Makefile.run
	@echo "	@echo \"--- [FASE 2] Reparaciones de Templates ---\"" >> Makefile.run
	@echo "	-grep -q \"typedef typename T::Type V;\" ../libraries/sp/coefficients.h || sed -i 's/typedef T::Type V;/typedef typename T::Type V;/g' ../libraries/sp/coefficients.h" >> Makefile.run
	@echo "	-grep -q \"typedef typename T::Type Widget;\" ../libraries/win/kali/ui/native/widgets.h || sed -i 's/typedef T::Type Widget;/typedef typename T::Type Widget;/g' ../libraries/win/kali/ui/native/widgets.h" >> Makefile.run
	@echo "	-grep -q \"typename Widget::Handle handle\" ../libraries/win/kali/ui/native/widgets.h || sed -i 's/Widget::Handle handle/typename Widget::Handle handle/g' ../libraries/win/kali/ui/native/widgets.h" >> Makefile.run
	@echo "	@echo \"--- [FASE 3] Correccion de Macro Plataforma ---\"" >> Makefile.run
	@echo "	-grep -q \"__PRETTY_FUNCTION__\" ../libraries/win/kali/platform.h || sed -i 's/__FUNCSIG__/__PRETTY_FUNCTION__/g' ../libraries/win/kali/platform.h" >> Makefile.run
	@echo "	@echo \"--- [FASE 4] CIRUGIA MAYOR RUNTIME.H ---\"" >> Makefile.run
	@echo "	-grep -q \"struct I2T\" ../libraries/kali/runtime.h || sed -i '/namespace kali {/a template<int N> struct I2T { enum { v = N }; };' ../libraries/kali/runtime.h" >> Makefile.run
	@echo "	-sed -i 's/template <E I> inline_ static void _(int offset)/template <int I> static void _(I2T<I>, int offset)/g' ../libraries/kali/runtime.h" >> Makefile.run
	@echo "	-sed -i 's/_<E(I + 1)>(offset)/_(I2T<I + 1>(), offset)/g' ../libraries/kali/runtime.h" >> Makefile.run
	@echo "	-sed -i 's/_<E(0)>(/_(I2T<0>(), /g' ../libraries/kali/runtime.h" >> Makefile.run
	@echo "	-sed -i 's/template <> static void _ <Size> (int) {}/static void _(I2T<N>, int) {}/g' ../libraries/kali/runtime.h" >> Makefile.run
	@echo "	-grep -q \"aux\[512\]\" ../libraries/kali/runtime.h || sed -i 's/static char aux\[\] = FUNCSIG_;/char aux[512]; strncpy(aux, FUNCSIG_, 511); aux[511]=0;/g' ../libraries/kali/runtime.h" >> Makefile.run
	@echo "	-sed -i 's/aux\[sizeof(aux)/ \\/\\/ aux[sizeof(aux)/g' ../libraries/kali/runtime.h" >> Makefile.run
	@echo "	@echo \"--- [FASE 5] ADELANTAMIENTO (Typenames) ---\"" >> Makefile.run
	@echo "	-grep -q \"typedef typename T::iterator\" ../libraries/kali/containers.h 2>/dev/null || sed -i 's/typedef T::iterator/typedef typename T::iterator/g' ../libraries/kali/containers.h" >> Makefile.run
	@echo "	-grep -q \"typedef typename T::Value\" ../libraries/kali/containers.h 2>/dev/null || sed -i 's/typedef T::Value/typedef typename T::Value/g' ../libraries/kali/containers.h" >> Makefile.run
	@echo "	-grep -q \"typedef typename T::\" ../libraries/win/kali/app.details.h || sed -i 's/typedef T::/typedef typename T::/g' ../libraries/win/kali/app.details.h" >> Makefile.run
	@echo "	-sed -i 's/Loaded<T>>/Loaded<T> >/g' ../libraries/win/kali/app.details.h" >> Makefile.run
	@echo "	@echo \"--- [FASE 6] FIX NAMESPACE APP.DETAILS (El error actual) ---\"" >> Makefile.run
	@echo "	-grep -q \"kali::ui::native::widget::Base::thunk_\" ../libraries/win/kali/app.details.h || sed -i 's/widget::Base::thunk_/kali::ui::native::widget::Base::thunk_/g' ../libraries/win/kali/app.details.h" >> Makefile.run
	@echo "" >> Makefile.run
	@echo "\$$(TARGET): \$$(OBJS)" >> Makefile.run
	@echo "	@echo \"--- ENLAZANDO DLL FINAL ---\"" >> Makefile.run
	@echo "	\$$(CXX) -shared -m32 -o \$$@ \$$(OBJS) \$$(LIBS) -Wl,--export-all-symbols -Wl,--kill-at -Wl,--defsym=main=VSTPluginMain" >> Makefile.run
	@echo "" >> Makefile.run
	@echo "vstsdk-wrapper.o: vstsdk-wrapper.cpp" >> Makefile.run
	@echo "	\$$(CXX) \$$(CXXFLAGS) \$$(INC) -c \$$< -o \$$@" >> Makefile.run
	@echo "" >> Makefile.run
	@echo "sp.o: sp.cpp" >> Makefile.run
	@echo "	\$$(CXX) \$$(CXXFLAGS) \$$(INC) -c \$$< -o \$$@" >> Makefile.run
	@echo "" >> Makefile.run
	@echo "main.res: main.rc" >> Makefile.run
	@echo "	windres main.rc -O coff -o main.res" >> Makefile.run
	@echo "" >> Makefile.run
	@echo "--- Ejecutando compilacion real ---"
	@make -f Makefile.run

clean:
	rm -f *.o *.res *.dll Makefile.run
