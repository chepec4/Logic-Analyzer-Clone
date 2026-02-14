# =============================================================================
# MAKEFILE DE CIRUGÍA (Arregla window.h y editor.h automáticamente)
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

# Objetivo principal: Primero parches, luego DLL
all: fix-code C4Analyzer.dll

# --- ZONA DE PARCHES (NO TOCAR) ---
# 1. analyzer.h: Agrega 'typename'
# 2. window.h: Usa string("%s", text) para evitar ambigüedad
# 3. sa.editor.h: Hace cast explícito a punteros (Combo*, Toggle*)
fix-code:
	@echo "--- Aplicando parches ---"
	-sed -i 's/Dst::T/typename Dst::T/g' analyzer.h
	-sed -i 's/!comments ? text/!comments ? string("%s", text)/g' ../libraries/win/kali/window.h
	-sed -i 's/Combo master(ctor(i));/Combo master((Combo*)ctor(i));/g' sa.editor.h
	-sed -i 's/Toggle master(ctor(i));/Toggle master((Toggle*)ctor(i));/g' sa.editor.h
	-sed -i 's/AnyWidget edit(ctor(i));/AnyWidget edit((AnyWidget*)ctor(i));/g' sa.editor.h

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
