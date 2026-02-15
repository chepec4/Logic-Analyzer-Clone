# =============================================================================
# MAKEFILE MAESTRO - C4 ANALYZER (LOGIC PRO PARITY)
# Optimizado para GitHub Actions y Alineación SSE3
# =============================================================================

CXX      := g++
WINDRES  := windres

# FLAGS DE INGENIERÍA:
# -mstackrealign: Crítico para estabilidad en DAWs (Logic/Cubase/etc)
# -msse3: Habilita el motor matemático optimizado
CXXFLAGS := -O3 -m32 -msse -msse2 -msse3 -std=gnu++98 -fpermissive \
            -w -mstackrealign -fno-exceptions -fno-rtti \
            -D_WIN32_WINNT=0x0501 -DBUILDING_VST=1 \
            -DNAME="\"C4 Analyzer\"" \
            -DCOMPANY="\"C4 Productions\""

# REGLA DE ORO: Agregamos ../libraries/win para que 'kali/platform.h' sea visible
INC      := -I. \
            -I../libraries \
            -I../libraries/win \
            -I../libraries/sp \
            -I../libraries/vst \
            -I../libraries/vst/public.sdk/source/vst2.x \
            -I../libraries/vst/pluginterfaces/vst2.x

# LIBRERÍAS DE SISTEMA
LIBS     := -static-libgcc -static-libstdc++ \
            -lgdi32 -luser32 -lcomdlg32 -lopengl32 -lglu32 -lwinmm -lole32 -lshlwapi

# OBJETOS
OBJS     := main.o vstsdk-wrapper.o main.res
TARGET   := ../C4Analyzer.dll

# .............................................................................

all: $(TARGET)

$(TARGET): $(OBJS)
	@echo "--- [LINKING] Enlazando DLL en la raíz del repositorio ---"
	$(CXX) -shared -m32 -o $@ $(OBJS) $(LIBS) -Wl,--kill-at

# REGLAS DE COMPILACIÓN (Sin prefijos de carpeta, archivos locales)
main.o: main.cpp
	@echo "--- [CXX] Compilando Motor Principal ---"
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

vstsdk-wrapper.o: vstsdk-wrapper.cpp
	@echo "--- [CXX] Compilando Interface VST ---"
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

main.res: rc/main.rc
	@echo "--- [RES] Compilando Recursos de Windows ---"
	$(WINDRES) -I. -Irc $< -O coff -o $@

clean:
	rm -f *.o *.res ../$(TARGET)

.PHONY: all clean


