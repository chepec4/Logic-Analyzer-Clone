# =============================================================================
# MAKEFILE MAESTRO - C4 ANALYZER
# Versión: V32.0 "The Final Stand" - Limpieza Absoluta y Corrección de Tipos
# =============================================================================

CXX      := g++
CXXFLAGS := -O2 -m32 -msse -msse2 -msse3 -std=gnu++98 -fpermissive -fno-access-control -w \
            -Wno-narrowing -Wno-write-strings -ftemplate-depth=3000 -mstackrealign \
            -D_WIN32_WINNT=0x0600 -DBUILDING_VST=1 \
            -DNAME="\"C4 Analyzer\"" \
            -DCOMPANY="\"C4 Productions\""

INC      := -I. -I../libraries -I../libraries/win -I../libraries/vst \
            -I../libraries/vst/public.sdk/source/vst2.x \
            -I../libraries/vst/pluginterfaces/vst2.x -I../libraries/sp

LIBS     := -static-libgcc -static-libstdc++ -lgdi32 -luser32 -lcomdlg32 -lopengl32 -lwinmm -lole32 -lshlwapi

OBJS     := vstsdk-wrapper.o sp.o main.res
TARGET   := C4Analyzer.dll

all: clean-slate restore-files fix-libraries rebuild-wrapper build-def $(TARGET)

clean-slate:
	rm -f *.o *.res *.dll fixer.pl plugin.def vstsdk-wrapper.cpp

restore-files:
	@echo "--- [0/4] Restaurando archivos de sistema ---"
	git checkout vstsdk-wrapper.cpp ../libraries/win/kali/window.h ../libraries/kali/dbgutils.h

fix-libraries:
	@echo "--- [1/4] Reparando ambigüedad de tipos en Kali ---"
	# Corrección definitiva: usamos la función string() de kali explícitamente para evitar ambigüedad
	sed -i 's/!comments ? text/!comments ? kali::string("%s", text)/g' ../libraries/win/kali/window.h
	# Reparación de macros de Debug
	sed -i 's/#define DBG (defined(_DEBUG))/#define DBG 0/g' ../libraries/kali/dbgutils.h
	# Reparación de punteros en runtime.h
	sed -i 's/\*dst++= \*src++;/*((char*\&)dst)++ = *((const char*\&)src)++;/g' ../libraries/kali/runtime.h

rebuild-wrapper:
	@echo "--- [2/4] Reconstruyendo Wrapper desde cero (Sin duplicados) ---"
	# Borramos el archivo y lo creamos nuevo para que NO haya dos VSTPluginMain
	echo '#include <windows.h>' > vstsdk-wrapper.cpp
	echo '#include "main.h"' >> vstsdk-wrapper.cpp
	echo 'extern "C" {' >> vstsdk-wrapper.cpp
	echo '  HINSTANCE hInstance = 0;' >> vstsdk-wrapper.cpp
	echo '  BOOL WINAPI DllMain(HINSTANCE h, DWORD r, LPVOID p) { if(r==1) hInstance=h; return TRUE; }' >> vstsdk-wrapper.cpp
	echo '  __declspec(dllexport) AEffect* VSTPluginMain(audioMasterCallback m) { if(!m) return 0; Plugin* e = new Plugin(m); return e ? e->getAeffect() : 0; }' >> vstsdk-wrapper.cpp
	echo '  __declspec(dllexport) AEffect* main(audioMasterCallback m) { return VSTPluginMain(m); }' >> vstsdk-wrapper.cpp
	echo '}' >> vstsdk-wrapper.cpp

build-def:
	@echo "--- [3/4] Generando Definition File ---"
	@echo "EXPORTS" > plugin.def
	@echo "    VSTPluginMain" >> plugin.def
	@echo "    main = VSTPluginMain" >> plugin.def

$(TARGET): $(OBJS)
	@echo "--- [4/4] Enlazando DLL Final ---"
	$(CXX) -shared -m32 -o $@ $(OBJS) $(LIBS) plugin.def -Wl,--kill-at
	@echo "--- COMPILACIÓN EXITOSA ---"

vstsdk-wrapper.o: vstsdk-wrapper.cpp
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

sp.o: sp.cpp
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

main.res: main.rc
	windres main.rc -O coff -o main.res

.PHONY: all clean-slate
