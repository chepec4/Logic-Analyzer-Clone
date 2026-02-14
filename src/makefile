# =============================================================================
# MAKEFILE MAESTRO - C4 ANALYZER
# Autor: ChepeC4 | C4 Productions
# Version: V30.0 "Total Integration" - Sincronía con Kali y SSE
# =============================================================================

CXX      := g++
# Agregamos -fno-stack-protector para evitar conflictos de memoria en VSTs antiguos
# Mantenemos -mstackrealign para que las funciones SSE no crasheen
CXXFLAGS := -O2 -m32 -msse -msse2 -msse3 -std=gnu++98 -fpermissive -fno-access-control -w \
            -Wno-narrowing -Wno-write-strings -ftemplate-depth=3000 -mstackrealign \
            -D_WIN32_WINNT=0x0600 -DBUILDING_VST=1 \
            -DNAME="\"C4 Analyzer\"" \
            -DCOMPANY="\"C4 Productions\"" \
            -DAUTHOR="\"ChepeC4\""

INC      := -I. -I../libraries -I../libraries/win -I../libraries/vst \
            -I../libraries/vst/public.sdk/source/vst2.x \
            -I../libraries/vst/pluginterfaces/vst2.x -I../libraries/sp

LIBS     := -static-libgcc -static-libstdc++ -lgdi32 -luser32 -lcomdlg32 -lopengl32 -lwinmm -lole32 -lshlwapi

OBJS     := vstsdk-wrapper.o sp.o main.res
TARGET   := C4Analyzer.dll

all: clean-slate restore-files fix-libraries patch-wrapper build-def $(TARGET)

clean-slate:
	rm -f *.o *.res *.dll fixer.pl plugin.def vstsdk-wrapper.cpp

restore-files:
	@echo "--- [0/4] Restaurando Wrapper Original ---"
	git checkout vstsdk-wrapper.cpp

fix-libraries:
	@echo "--- [1/4] Cirugía en Runtime (SSE Safety) ---"
	@echo 'use strict; use warnings; my $$f = $$ARGV[0]; local $$/ = undef; open my $$in, "<", $$f; my $$c = <$$in>; close $$in;' > fixer.pl
	@echo 'if ($$f =~ /runtime.h/) { $$c =~ s/namespace kali \{/namespace kali { template<int N> struct I2T { enum { v = N }; };/ unless $$c =~ /struct I2T/;' >> fixer.pl
	@echo '$$c =~ s/template\s*<E\s*I>\s*inline_\s*static\s*void\s*_\s*\(int\s*offset\)/template <int I> inline_ static void _(I2T<I>, int offset)/g;' >> fixer.pl
	@echo '$$c =~ s/_\s*<E\s*\(\s*I\s*\+\s*1\s*\)\s*>\s*\(\s*offset\s*\)/_(I2T<I + 1>(), offset)/g;' >> fixer.pl
	@echo '$$c =~ s/_\s*<E\s*\(\s*0\s*\)\s*>\s*\(/_(I2T<0>(), /g;' >> fixer.pl
	@echo '$$c =~ s/template\s*<>\s*static\s*void\s*_\s*<Size>\s*\(int\)\s*{}/static void _(I2T<(int)Size>, int) {}/g;' >> fixer.pl
	@echo '$$c =~ s/static\s*char\s*aux\[\]\s*=\s*FUNCSIG_;/static const char* aux = FUNCSIG_;/g; $$c =~ s/(aux\[sizeof\(aux\).*=.*0;)/\/\/ $$1/g; }' >> fixer.pl
	@echo 'open my $$out, ">", $$f; print $$out $$c; close $$out;' >> fixer.pl
	perl fixer.pl ../libraries/kali/runtime.h

patch-wrapper:
	@echo "--- [2/4] Integrando DllMain y VST Entry ---"
	# Renombramos main para evitar conflictos de tipo con GCC
	sed -i 's/AEffect\s*\*\s*main/AEffect* Main_Old/g' vstsdk-wrapper.cpp
	@echo "" >> vstsdk-wrapper.cpp
	@echo 'extern "C" {' >> vstsdk-wrapper.cpp
	@echo '  // Esta variable conecta con sa.display.h y kali' >> vstsdk-wrapper.cpp
	@echo '  HINSTANCE hInstance = 0;' >> vstsdk-wrapper.cpp
	@echo '  BOOL WINAPI DllMain(HINSTANCE h, DWORD r, LPVOID p) { if(r==1) hInstance=h; return TRUE; }' >> vstsdk-wrapper.cpp
	@echo '  __declspec(dllexport) AEffect* VSTPluginMain(audioMasterCallback m) { ' >> vstsdk-wrapper.cpp
	@echo '    if(!m) return 0; Plugin* e = new Plugin(m); return e ? e->getAeffect() : 0;' >> vstsdk-wrapper.cpp
	@echo '  }' >> vstsdk-wrapper.cpp
	@echo '}' >> vstsdk-wrapper.cpp

build-def:
	@echo "--- [3/4] Generando Mapa de Exportación ---"
	@echo "EXPORTS" > plugin.def
	@echo "    VSTPluginMain" >> plugin.def
	@echo "    main = VSTPluginMain" >> plugin.def

$(TARGET): $(OBJS)
	@echo "--- [4/4] Enlazando DLL ---"
	$(CXX) -shared -m32 -o $@ $(OBJS) $(LIBS) plugin.def -Wl,--kill-at
	@echo "--- COMPILACIÓN V30 FINALIZADA ---"

vstsdk-wrapper.o: vstsdk-wrapper.cpp
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

sp.o: sp.cpp
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

main.res: main.rc
	windres main.rc -O coff -o main.res

.PHONY: all clean-slate
