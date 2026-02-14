# =============================================================================
# MAKEFILE DE RESCATE (Regreso a la configuración 32-bit que funcionaba)
# =============================================================================

CXX      := g++
# Usamos gnu++98 para máxima compatibilidad con el código viejo (Kali)
CXXFLAGS := -O2 -m32 -std=gnu++98 -fpermissive -w -D_WIN32_WINNT=0x0600 -DBUILDING_VST=1

INC      := -I. \
            -I../libraries \
            -I../libraries/win \
            -I../libraries/vst \
            -I../libraries/vst/public.sdk/source/vst2.x \
            -I../libraries/vst/pluginterfaces/vst2.x \
            -I../libraries/sp

LIBS     := -static-libgcc -static-libstdc++ -lgdi32 -luser32 -lcomdlg32 -lopengl32 -lwinmm

OBJS     := vstsdk-wrapper.o sp.o main.res

# Objetivo con parches previos
all: patch-window fix-analyzer C4Analyzer.dll

# PARCHE 1: Soluciona el error de "ambiguous call to String" en window.h
# Forzamos el constructor String("%s", text) para que no haya duda
patch-window:
	@echo "--- Arreglando window.h ---"
	-sed -i 's/!comments ? text/!comments ? string("%s", text)/g' ../libraries/win/kali/window.h || true

# PARCHE 2: Agrega typename faltante en analyzer.h (requerido por GCC moderno)
fix-analyzer:
	@echo "--- Arreglando analyzer.h ---"
	-sed -i 's/Dst::T/typename Dst::T/g' analyzer.h || true

C4Analyzer.dll: $(OBJS)
	$(CXX) -shared -m32 -o $@ $(OBJS) $(LIBS) -Wl,--export-all-symbols -Wl,--kill-at

vstsdk-wrapper.o: vstsdk-wrapper.cpp
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

sp.o: sp.cpp
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

main.res: main.rc
	windres main.rc -O coff -o main.res

clean:
	rm -f *.o *.res *.dll
