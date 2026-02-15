# -----------------------------------------------------------------------------
# C4 ANALYZER - MAKEFILE (V8.0 - Definitive Infrastructure Build)
# Optimizado para MinGW-w64 (GCC 12.2+) - Entorno x64
# -----------------------------------------------------------------------------

CXX      := g++
WINDRES  := windres
RM       := rm -f

# -----------------------------------------------------------------------------
# FLAGS DE COMPILACIÓN (Motor DSP y Compatibilidad)
# -----------------------------------------------------------------------------
# -O3 + -ffast-math: Rendimiento crítico para el motor de audio SIMD.
# -mstackrealign: Vital para que las instrucciones SSE en sp.h no fallen en x86/x64.
# -ftemplate-depth=2000: Evita colapsos en la jerarquía de widgets de Kali.
# -----------------------------------------------------------------------------
CXXFLAGS := -O3 -ffast-math -std=gnu++14 -fpermissive -w \
            -msse -msse2 -msse3 -mstackrealign \
            -ftemplate-depth=2000 \
            -Wno-narrowing -Wno-attributes \
            -static-libgcc -static-libstdc++ \
            -DNDEBUG -DWINDOWS_=1 -D_WIN32_WINNT=0x0600 \
            -D__GTHREADS

# -----------------------------------------------------------------------------
# RUTAS DE INCLUSIÓN (Estructura de Capas)
# -----------------------------------------------------------------------------
INC      := -I. \
            -I../libraries \
            -I../libraries/win \
            -I../libraries/sp \
            -I../libraries/vst \
            -I../libraries/vst/public.sdk/source/vst2.x \
            -I../libraries/vst/pluginterfaces/vst2.x

# -----------------------------------------------------------------------------
# LIBRERÍAS DEL SISTEMA Y ENLAZADO
# -----------------------------------------------------------------------------
# Orden crítico: comctl32 es necesaria para los widgets (Tabs, Meters).
# winmm es necesaria para el timing de precisión del analizador.
LIBS      := -lgdi32 -luser32 -lkernel32 -lcomctl32 -lcomdlg32 -lwinmm -lole32 -luuid -loleaut32
LDFLAGS   := -shared -s -Wl,--kill-at -Wl,--subsystem,windows -Wl,--add-stdcall-alias

# -----------------------------------------------------------------------------
# ARCHIVOS DEL PROYECTO
# -----------------------------------------------------------------------------
SRCS      := main.cpp vstsdk-wrapper.cpp
OBJS      := $(SRCS:.cpp=.o)
RES       := main.res
TARGET    := ../C4Analyzer.dll

# -----------------------------------------------------------------------------
# REGLAS DE CONSTRUCCIÓN
# -----------------------------------------------------------------------------

all: $(TARGET)



# [LINKER] Generación del binario VST DLL
$(TARGET): $(OBJS) $(RES)
	@echo [LINKER] Ensamblando binario final: $@
	$(CXX) $(LDFLAGS) -o $@ $(OBJS) $(RES) $(LIBS)

# [CXX] Compilación de módulos C++
%.o: %.cpp
	@echo [CXX] Procesando: $<
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

# [RC] Compilación de Recursos (Iconos, Versión, Manifiesto)
$(RES): rc/main.rc
	@echo [RC] Compilando recursos: $<
	$(WINDRES) $(INC) -Irc -i $< -O coff -o $@

# Limpieza profunda
clean:
	@echo [CLEAN] Eliminando objetos y binarios...
	$(RM) $(OBJS) $(RES) $(TARGET)

.PHONY: all clean
