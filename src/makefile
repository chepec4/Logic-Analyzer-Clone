# =============================================================================
# MAKEFILE MAESTRO - C4 ANALYZER (LOGIC PRO PARITY)
# Compilador: MinGW / GCC (Target x86 32-bit)
# =============================================================================

CXX      := g++
WINDRES  := windres

# REGLA DE ORO: Flags optimizados para el motor SSE del Punto 1
# -mstackrealign: Crítico para que el DAW y el plugin compartan la pila sin crashes
# -msse3: Necesario para la función hsum(__m128)
CXXFLAGS := -O3 -m32 -msse -msse2 -msse3 -std=gnu++98 -fpermissive \
            -w -mstackrealign -fno-exceptions -fno-rtti \
            -D_WIN32_WINNT=0x0501 -DBUILDING_VST=1 \
            -DNAME="\"C4 Analyzer\"" \
            -DCOMPANY="\"C4 Productions\""

# RUTAS DE INCLUSIÓN (Meticulosidad en la jerarquía)
INC      := -I. -I./src -I../libraries -I../libraries/sp -I../libraries/win \
            -I../libraries/vst -I../libraries/vst/public.sdk/source/vst2.x \
            -I../libraries/vst/pluginterfaces/vst2.x

# LIBRERÍAS DE SISTEMA (Soporte OpenGL para sa.display.h)
LIBS     := -static-libgcc -static-libstdc++ \
            -lgdi32 -luser32 -lcomdlg32 -lopengl32 -lglu32 -lwinmm -lole32 -lshlwapi

# OBJETOS (Orden de compilación para evitar símbolos huérfanos)
OBJS     := main.o vstsdk-wrapper.o main.res
TARGET   := C4Analyzer.dll

# .............................................................................

all: $(TARGET)

$(TARGET): $(OBJS)
	@echo "--- [LINKING] Generando DLL alineada para VST ---"
	$(CXX) -shared -m32 -o $@ $(OBJS) $(LIBS) -Wl,--kill-at
	@echo "--- [SUCCESS] C4 Analyzer listo para GitHub ---"

# Compilación del punto de entrada del Sistema
main.o: src/main.cpp
	@echo "--- [CXX] Compilando Motor Principal ---"
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

# Compilación del Wrapper VST (Punto de entrada al DAW)
vstsdk-wrapper.o: src/vstsdk-wrapper.cpp
	@echo "--- [CXX] Compilando Interface VST ---"
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

# Recursos (Iconos y versión para el Explorador de Windows)
main.res: src/rc/main.rc
	@echo "--- [RES] Compilando Recursos de Windows ---"
	$(WINDRES) -Isrc -Isrc/rc $< -O coff -o $@

clean:
	@echo "--- [CLEAN] Limpiando entorno de construcción ---"
	rm -f *.o *.res *.dll

.PHONY: all clean
