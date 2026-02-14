# =============================================================================
# MAKEFILE MAESTRO - C4 ANALYZER
# =============================================================================

CXX      := g++
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

# Objetivo: Reparar -> Compilar
all: fix-everything C4Analyzer.dll

# ZONA DE PARCHES (NO MODIFICAR)
fix-everything:
	@echo "--- Ejecutando reparaciones de codigo ---"
	# 1. Arreglar typo en Kali App
	-sed -i 's/typedef HINSTANCE__ Module;/typedef struct HINSTANCE__ Module;/g' ../libraries/kali/app.h
	# 2. Arreglar aritmetica void* en Kali Runtime
	-sed -i 's/\*dst++= \*src++;/*((char*\&)dst)++ = *((const char*\&)src)++;/g' ../libraries/kali/runtime.h
	# 3. Arreglar ambiguedad en Kali Window (String explícito)
	-sed -i 's/!comments ? text/!comments ? string("%s", text)/g' ../libraries/win/kali/window.h
	# 4. Eliminar DBG ilegal
	-sed -i 's/#define DBG (defined(_DEBUG))/\/\/#define DBG (defined(_DEBUG))/g' ../libraries/kali/dbgutils.h
	# 5. Arreglar Analyzer Typename
	-sed -i 's/Dst::T/typename Dst::T/g' analyzer.h
	# 6. Arreglar Ambiguedad en Editor (Casting explicito)
	-sed -i 's/Combo master(ctor(i));/Combo master((Combo*)ctor(i));/g' sa.editor.h
	-sed -i 's/Toggle master(ctor(i));/Toggle master((Toggle*)ctor(i));/g' sa.editor.h
	-sed -i 's/AnyWidget edit(ctor(i));/AnyWidget edit((AnyWidget*)ctor(i));/g' sa.editor.h
	@echo "--- Codigo reparado ---"

# Linker: Alias main -> VSTPluginMain
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
