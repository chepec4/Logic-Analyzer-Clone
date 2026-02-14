# =============================================================================
# MAKEFILE MAESTRO - C4 ANALYZER
# Autor: ChepeC4 | C4 Productions
# Version: V25.0 "Lazarus" - Restauracion de DllMain y hInstance
# =============================================================================

CXX      := g++
# Mantenemos 32-bit compatible con Bridge
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
all: fix-libraries fix-wrapper fix-def $(TARGET)

# =============================================================================
# PASO 1: REPARACION DE LIBRERIAS (Perl Scripting)
# =============================================================================
fix-libraries:
	@echo "--- [1/3] Ejecutando Reparaciones de Librerias ---"
	@echo 'use strict; use warnings;' > fixer.pl
	@echo 'my $$file = $$ARGV[0];' >> fixer.pl
	@echo 'local $$/ = undef;' >> fixer.pl
	@echo 'open my $$in, "<", $$file or die "Can not open $$file";' >> fixer.pl
	@echo 'my $$content = <$$in>;' >> fixer.pl
	@echo 'close $$in;' >> fixer.pl
	@echo '' >> fixer.pl
	@echo 'if ($$file =~ /runtime.h/) {' >> fixer.pl
	@echo '  $$content =~ s/namespace kali \{/namespace kali { template<int N> struct I2T { enum { v = N }; };/ unless $$content =~ /struct I2T/;' >> fixer.pl
	@echo '  $$content =~ s/template\s*<E\s*I>\s*inline_\s*static\s*void\s*_\s*\(int\s*offset\)/template <int I> inline_ static void _(I2T<I>, int offset)/g;' >> fixer.pl
	@echo '  $$content =~ s/_\s*<E\s*\(\s*I\s*\+\s*1\s*\)\s*>\s*\(\s*offset\s*\)/_(I2T<I + 1>(), offset)/g;' >> fixer.pl
	@echo '  $$content =~ s/_\s*<E\s*\(\s*0\s*\)\s*>\s*\(/_(I2T<0>(), /g;' >> fixer.pl
	@echo '  $$content =~ s/_\(I2T<0>\(\),\s*\(/_(I2T<0>(), /g;' >> fixer.pl
	@echo '  $$content =~ s/_\(I2T<0>\(\),\s*\"_\"\)/_(I2T<0>(), sizeof(FUNCSIG_) + sizeof("_") - sizeof("ctor(void)"))/g;' >> fixer.pl
	@echo '  $$content =~ s/template\s*<>\s*static\s*void\s*_\s*<Size>\s*\(int\)\s*{}/static void _(I2T<(int)Size>, int) {}/g;' >> fixer.pl
	@echo '  $$content =~ s/static\s*char\s*aux\[\]\s*=\s*FUNCSIG_;/static const char* aux = FUNCSIG_;/g;' >> fixer.pl
	@echo '  $$content =~ s/(aux\[sizeof\(aux\).*=.*0;)/\/\/ $$1/g;' >> fixer.pl
	@echo '}' >> fixer.pl
	@echo 'if ($$file =~ /app.details.h/) {' >> fixer.pl
	@echo '  $$content =~ s/widget::Base::thunk_/kali::ui::native::widget::Base::thunk_/g;' >> fixer.pl
	@echo '  $$content =~ s/widget::Base::drawThunk_/kali::ui::native::widget::Base::drawThunk_/g;' >> fixer.pl
	@echo '  $$content =~ s/typedef\s+T::/typedef typename T::/g;' >> fixer.pl
	@echo '  $$content =~ s/Loaded<T>>/Loaded<T> >/g;' >> fixer.pl
	@echo '}' >> fixer.pl
	@echo 'open my $$out, ">", $$file or die "Can not write $$file";' >> fixer.pl
	@echo 'print $$out $$content;' >> fixer.pl
	@echo 'close $$out;' >> fixer.pl

	-grep -q "typedef struct HINSTANCE__" ../libraries/kali/app.h || sed -i 's/typedef.*HINSTANCE__.*Module;/typedef struct HINSTANCE__ Module;/g' ../libraries/kali/app.h
	-grep -q "((char*&)" ../libraries/kali/runtime.h || sed -i 's/\*dst++= \*src++;/*((char*\&)dst)++ = *((const char*\&)src)++;/g' ../libraries/kali/runtime.h
	-grep -q "string(\"%s\"" ../libraries/win/kali/window.h || sed -i 's/!comments ? text/!comments ? string("%s", text)/g' ../libraries/win/kali/window.h
	-grep -q "//#define DBG" ../libraries/kali/dbgutils.h || sed -i 's/#define DBG (defined(_DEBUG))/\/\/#define DBG (defined(_DEBUG))/g' ../libraries/kali/dbgutils.h
	-grep -q "typedef typename T::Type V;" ../libraries/sp/coefficients.h || sed -i 's/typedef.*T::Type.*V;/typedef typename T::Type V;/g' ../libraries/sp/coefficients.h
	-grep -q "typedef typename T::Type Widget;" ../libraries/win/kali/ui/native/widgets.h || sed -i 's/typedef.*T::Type.*Widget;/typedef typename T::Type Widget;/g' ../libraries/win/kali/ui/native/widgets.h
	-grep -q "typename Widget::Handle handle" ../libraries/win/kali/ui/native/widgets.h || sed -i 's/Widget::Handle.*handle/typename Widget::Handle handle/g' ../libraries/win/kali/ui/native/widgets.h
	-grep -q "__PRETTY_FUNCTION__" ../libraries/win/kali/platform.h || sed -i 's/__FUNCSIG__/__PRETTY_FUNCTION__/g' ../libraries/win/kali/platform.h
	-grep -q "typedef typename T::iterator" ../libraries/kali/containers.h 2>/dev/null || sed -i 's/typedef.*T::iterator/typedef typename T::iterator/g' ../libraries/kali/containers.h
	-grep -q "typedef typename T::Value" ../libraries/kali/containers.h 2>/dev/null || sed -i 's/typedef.*T::Value/typedef typename T::Value/g' ../libraries/kali/containers.h

	perl fixer.pl ../libraries/kali/runtime.h
	perl fixer.pl ../libraries/win/kali/app.details.h

# =============================================================================
# PASO 2: REESCRITURA DE WRAPPER (CON DLLMAIN PARA GUI)
# =============================================================================
fix-wrapper:
	@echo "--- [2/3] Regenerando vstsdk-wrapper.cpp con DllMain ---"
	@echo '#define _CRT_SECURE_NO_WARNINGS' > vstsdk-wrapper.cpp
	@echo '#include <windows.h>' >> vstsdk-wrapper.cpp
	@echo '#include <commdlg.h>' >> vstsdk-wrapper.cpp
	@echo '#include <objidl.h>' >> vstsdk-wrapper.cpp
	@echo '#include <shlwapi.h>' >> vstsdk-wrapper.cpp
	@echo '#include "main.h"' >> vstsdk-wrapper.cpp
	@echo '#include "audioeffect.cpp"' >> vstsdk-wrapper.cpp
	@echo '#include "audioeffectx.cpp"' >> vstsdk-wrapper.cpp
	@echo '' >> vstsdk-wrapper.cpp
	@echo '// VARIABLE GLOBAL NECESARIA PARA LA GUI' >> vstsdk-wrapper.cpp
	@echo 'void* hInstance = 0;' >> vstsdk-wrapper.cpp
	@echo '' >> vstsdk-wrapper.cpp
	@echo '// PUNTO DE ENTRADA DE WINDOWS (RESUCITA LA GUI)' >> vstsdk-wrapper.cpp
	@echo 'extern "C" BOOL WINAPI DllMain(HINSTANCE hInst, DWORD dwReason, LPVOID lpvReserved) {' >> vstsdk-wrapper.cpp
	@echo '    if (dwReason == DLL_PROCESS_ATTACH) {' >> vstsdk-wrapper.cpp
	@echo '        hInstance = hInst;' >> vstsdk-wrapper.cpp
	@echo '    }' >> vstsdk-wrapper.cpp
	@echo '    return 1;' >> vstsdk-wrapper.cpp
	@echo '}' >> vstsdk-wrapper.cpp
	@echo '' >> vstsdk-wrapper.cpp
	@echo 'extern "C" {' >> vstsdk-wrapper.cpp
	@echo '    __declspec(dllexport) AEffect* VSTPluginMain(audioMasterCallback master) {' >> vstsdk-wrapper.cpp
	@echo '        if (!master) return 0;' >> vstsdk-wrapper.cpp
	@echo '        Plugin* effect = new Plugin(master);' >> vstsdk-wrapper.cpp
	@echo '        if (!effect) return 0;' >> vstsdk-wrapper.cpp
	@echo '        return effect->getAeffect();' >> vstsdk-wrapper.cpp
	@echo '    }' >> vstsdk-wrapper.cpp
	@echo '    // Proxy para alias main' >> vstsdk-wrapper.cpp
	@echo '    __declspec(dllexport) AEffect* Main_Proxy(audioMasterCallback master) {' >> vstsdk-wrapper.cpp
	@echo '        return VSTPluginMain(master);' >> vstsdk-wrapper.cpp
	@echo '    }' >> vstsdk-wrapper.cpp
	@echo '}' >> vstsdk-wrapper.cpp

# =============================================================================
# PASO 3: ARCHIVO DEF
# =============================================================================
fix-def:
	@echo "--- [3/3] Creando C4Analyzer.def ---"
	@echo "EXPORTS" > C4Analyzer.def
	@echo "    VSTPluginMain" >> C4Analyzer.def
	@echo "    main = Main_Proxy" >> C4Analyzer.def

# =============================================================================
# COMPILACION FINAL
# =============================================================================

$(TARGET): $(OBJS)
	@echo "--- ENLAZANDO DLL (Con DllMain y Exports) ---"
	$(CXX) -shared -m32 -o $@ $(OBJS) $(LIBS) C4Analyzer.def -Wl,--kill-at
	@echo "--- EXITO: C4 Analyzer listo para GUI ---"

vstsdk-wrapper.o: vstsdk-wrapper.cpp
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

sp.o: sp.cpp
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

main.res: main.rc
	windres main.rc -O coff -o main.res

clean:
	rm -f *.o *.res *.dll fixer.pl C4Analyzer.def vstsdk-wrapper.cpp

.PHONY: all fix-libraries fix-wrapper fix-def clean
