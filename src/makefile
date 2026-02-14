# =============================================================================
# MAKEFILE AUTOCORRECTOR - C4 ANALYZER
# Parchea código viejo al vuelo y compila con soporte VST completo
# =============================================================================

CXX      := g++
CXXFLAGS := -O2 -m32 -msse -msse2 -msse3 -std=gnu++03 -fpermissive -w -D_WIN32_WINNT=0x0600 -DBUILDING_VST=1

INC      := -I. \
            -I../libraries \
            -I../libraries/win \
            -I../libraries/vst \
            -I../libraries/vst/public.sdk/source/vst2.x \
            -I../libraries/vst/pluginterfaces/vst2.x \
            -I../libraries/sp

LIBS     := -static-libgcc -static-libstdc++ -lgdi32 -luser32 -lcomdlg32 -lopengl32 -lwinmm

OBJS     := vstsdk-wrapper.o sp.o main.res

# Objetivo Principal: Primero parches, luego DLL
all: patch-files C4Analyzer.dll

# REGLA DE PARCHEO (La Magia)
# 1. Agrega 'typename' donde falta en analyzer.h
# 2. Arregla el error ternario (? :) en window.h convirtiendo char* a string
patch-files:
	@echo "--- Aplicando parches automáticos ---"
	-sed -i 's/Dst::T/typename Dst::T/g' analyzer.h
	-sed -i 's/!comments ? text/!comments ? string(text)/g' ../libraries/win/kali/window.h
	@echo "--- Parches aplicados ---"

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
