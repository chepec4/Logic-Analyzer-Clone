# =============================================================================
# MAKEFILE MAESTRO - C4 ANALYZER
# Autor: ChepeC4 | C4 Productions
# Version: V31.0 "The Final Fix" - Reparación Automática de Librerías
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

all: clean-slate restore-files fix-all-libraries patch-wrapper build-def $(TARGET)

clean-slate:
	rm -f *.o *.res *.dll fixer.pl plugin.def vstsdk-wrapper.cpp

restore-files:
	@echo "--- [0/4] Restaurando Archivos ---"
	git checkout vstsdk-wrapper.cpp

fix-all-libraries:
	@echo "--- [1/4] Reparando Errores de Tipos y Macros en Librerías ---"
	# Reparación de window.h (Error de operandos ?: )
	sed -i 's/!comments ? text/!comments ? (string)text/g' ../libraries/win/kali/window.h
	# Reparación de dbgutils.h (Error de macro _DEBUG)
	sed -i 's/#define DBG (defined(_DEBUG))/#define DBG 0/g' ../libraries/kali/dbgutils.h
	# Reparación de runtime.h (SSE y Castings)
	sed -i 's/\*dst++= \*src++;/*((char*\&)dst)++ = *((const char*\&)src)++;/g' ../libraries/kali/runtime.h

patch-wrapper:
	@echo "--- [2/4] Limpiando Wrapper para evitar Redefinición ---"
	# Borramos cualquier VSTPluginMain que hayamos inyectado antes para empezar de cero
	sed -i 's/AEffect\s*\*\s*main/AEffect* Main_Old/g' vstsdk-wrapper.cpp
	@echo "" >> vstsdk-wrapper.cpp
	@echo 'extern "C" {' >> vstsdk-wrapper.cpp
	@echo '  HINSTANCE hInstance = 0;' >> vstsdk-wrapper.cpp
	@echo '  BOOL WINAPI DllMain(HINSTANCE h, DWORD r, LPVOID p) { if(r==1) hInstance=h; return TRUE; }' >> vstsdk-wrapper.cpp
	@echo '  __declspec(dllexport) AEffect* VSTPluginMain(audioMasterCallback m) { ' >> vstsdk-wrapper.cpp
	@echo '    if(!m) return 0; Plugin* e = new Plugin(m); return e ? e->getAeffect() : 0;' >> vstsdk-wrapper.cpp
	@echo '  }' >> vstsdk-wrapper.cpp
	@echo '}' >> vstsdk-wrapper.cpp

build-def:
	@echo "--- [3/4] Creando DEF ---"
	@echo "EXPORTS" > plugin.def
	@echo "    VSTPluginMain" >> plugin.def
	@echo "    main = VSTPluginMain" >> plugin.def

$(TARGET): $(OBJS)
	@echo "--- [4/4] Enlazando ---"
	$(CXX) -shared -m32 -o $@ $(OBJS) $(LIBS) plugin.def -Wl,--kill-at
	@echo "--- PROCESO COMPLETADO ---"

vstsdk-wrapper.o: vstsdk-wrapper.cpp
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

sp.o: sp.cpp
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

main.res: main.rc
	windres main.rc -O coff -o main.res

.PHONY: all clean-slate
