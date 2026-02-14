# =============================================================================
# MAKEFILE FINAL - C4 ANALYZER
# Incluye parches automáticos para Kali Framework y SP Audio Lib
# =============================================================================

CXX      := g++
# Flag Mágica: -fno-access-control (Permite acceder a miembros privados, vital para sa.editor.h)
CXXFLAGS := -O2 -m32 -msse -msse2 -msse3 -std=gnu++98 -fpermissive -fno-access-control -w -D_WIN32_WINNT=0x0600 -DBUILDING_VST=1

INC      := -I. \
            -I../libraries \
            -I../libraries/win \
            -I../libraries/vst \
            -I../libraries/vst/public.sdk/source/vst2.x \
            -I../libraries/vst/pluginterfaces/vst2.x \
            -I../libraries/sp

LIBS     := -static-libgcc -static-libstdc++ -lgdi32 -luser32 -lcomdlg32 -lopengl32 -lwinmm -lole32 -lshlwapi

OBJS     := vstsdk-wrapper.o sp.o main.res

# Objetivo: Parchear todo -> Compilar DLL
all: fix-all C4Analyzer.dll

# ZONA DE PARCHES
fix-all:
	@echo "--- Aplicando Parches ---"
	# 1. Arreglar typo en kali/app.h (HINSTANCE__ es struct, no tipo directo)
	-sed -i 's/typedef HINSTANCE__ Module;/typedef struct HINSTANCE__ Module;/g' ../libraries/kali/app.h
	# 2. Arreglar punteros void en kali/runtime.h (Castear a char*)
	-sed -i 's/\*dst++= \*src++;/*((char*\&)dst)++ = *((const char*\&)src)++;/g' ../libraries/kali/runtime.h
	# 3. Arreglar ambigüedad en window.h
	-sed -i 's/!comments ? text/!comments ? string("%s", text)/g' ../libraries/win/kali/window.h
	# 4. Arreglar typename en analyzer.h
	-sed -i 's/Dst::T/typename Dst::T/g' analyzer.h
	@echo "--- Parches Listos ---"

# Linker con alias de exportación para main
C4Analyzer.dll: $(OBJS)
	$(CXX) -shared -m32 -o $@ $(OBJS) $(LIBS) -Wl,--export-all-symbols -Wl,--kill-at -Wl,--defsym=main=VSTPluginMain

vstsdk-wrapper.o: vstsdk-wrapper.cpp
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

sp.o: sp.cpp
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

main.res: main.rc
	windres main.rc -O coff -o main.res

clean:
	rm -f *.o *.res *.dll
