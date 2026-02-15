# =============================================================================
# MAKEFILE MAESTRO - C4 ANALYZER (LOGIC PRO PARITY)
# Optimizado para MinGW 32-bit y Estructura RC
# =============================================================================

CXX      := i686-w64-mingw32-g++
WINDRES  := windres

# FLAGS DE INGENIERÍA:
# -mstackrealign: Crítico para estabilidad en DAWs
# -msse3: Motor de audio optimizado
CXXFLAGS := -O3 -m32 -msse -msse2 -msse3 -std=gnu++98 -fpermissive \
            -w -mstackrealign -fno-exceptions -fno-rtti \
            -D_WIN32_WINNT=0x0501 -DBUILDING_VST=1 \
            -DNAME="\"C4 Analyzer\"" \
            -DCOMPANY="\"C4 Productions\""

# RUTAS DE INCLUSIÓN (CRÍTICO: ../libraries/win soluciona el error kali/platform.h)
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

# REGLAS DE COMPILACIÓN

# 1. Main.cpp (Lógica Principal)
main.o: main.cpp
	@echo "--- [CXX] Compilando Motor Principal ---"
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

# 2. Wrapper VST (Comunicación con el DAW)
vstsdk-wrapper.o: vstsdk-wrapper.cpp
	@echo "--- [CXX] Compilando Interface VST ---"
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

# 3. Recursos (El archivo maestro está en rc/main.rc)
main.res: rc/main.rc
	@echo "--- [RES] Compilando Recursos (Manifest, Dialogs, Version) ---"
	$(WINDRES) $(INC) -Irc -i $< -O coff -o $@

clean:
	@echo "--- [CLEAN] Limpiando binarios intermedios ---"
	rm -f *.o *.res ../$(TARGET)

.PHONY: all clean
