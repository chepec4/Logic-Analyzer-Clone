# -----------------------------------------------------------------------------
# C4 ANALYZER - MAKEFILE (V7.0 - Final Precision Build)
# Optimizado para MinGW-w64 (GCC 12.2+) - Entorno 2026
# -----------------------------------------------------------------------------

CXX      := g++
WINDRES  := windres
RM       := rm -f

# -----------------------------------------------------------------------------
# FLAGS DE COMPILACION (Motor DSP y Compatibilidad)
# -----------------------------------------------------------------------------
# -O3 + -ffast-math: Rendimiento critico para el motor de audio SIMD.
# -mstackrealign: Garantiza que las instrucciones SSE en sp.h no fallen.
# -fpermissive: Red de seguridad para el codigo de la libreria VST SDK.
# -ftemplate-depth=1000: Seguro tras simplificar kali/runtime.h.
# -----------------------------------------------------------------------------
CXXFLAGS := -O3 -ffast-math -std=gnu++14 -fpermissive -w \
            -msse -msse2 -msse3 -mstackrealign \
            -ftemplate-depth=1000 \
            -Wno-narrowing \
            -static-libgcc -static-libstdc++ \
            -DNDEBUG -DWINDOWS_=1 -D_WIN32_WINNT=0x0600 \
            -D__GTHREADS

# -----------------------------------------------------------------------------
# RUTAS DE INCLUSION
# -----------------------------------------------------------------------------
INC      := -I. \
            -I../libraries \
            -I../libraries/win \
            -I../libraries/sp \
            -I../libraries/vst \
            -I../libraries/vst/public.sdk/source/vst2.x \
            -I../libraries/vst/pluginterfaces/vst2.x

# -----------------------------------------------------------------------------
# LIBRERIAS DEL SISTEMA Y ENLAZADO
# -----------------------------------------------------------------------------
# El orden es vital: gdi32 y opengl32 para la UI, winmm para el timing de audio.
LIBS     := -lgdi32 -luser32 -lkernel32 -lcomdlg32 -lopengl32 -lglu32 -lwinmm -lole32 -luuid -loleaut32
LDFLAGS  := -shared -s -Wl,--kill-at -Wl,--subsystem,windows

# -----------------------------------------------------------------------------
# ARCHIVOS DEL PROYECTO
# -----------------------------------------------------------------------------
SRCS     := main.cpp vstsdk-wrapper.cpp
OBJS     := $(SRCS:.cpp=.o)
RES      := main.res
TARGET   := ../C4Analyzer.dll

# -----------------------------------------------------------------------------
# REGLAS DE CONSTRUCCION
# -----------------------------------------------------------------------------

all: $(TARGET)

# [LINKER] Generacion del binario VST DLL
# Se asegura que LIBS este al final para resolver dependencias correctamente.
$(TARGET): $(OBJS) $(RES)
	@echo [LINKER] Ensamblando binario final: $@
	$(CXX) $(LDFLAGS) -o $@ $(OBJS) $(RES) $(LIBS)

# [CXX] Compilacion de modulos C++
%.o: %.cpp
	@echo [CXX] Procesando: $<
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

# [RC] Compilacion de Recursos (Iconos, Version, Manifiesto)
# Incluye INC para que windres pueda localizar archivos de cabecera si es necesario.
$(RES): rc/main.rc
	@echo [RC] Compilando recursos: $<
	$(WINDRES) $(INC) -Irc -i $< -O coff -o $@

# Limpieza profunda
clean:
	@echo [CLEAN] Eliminando objetos y binarios...
	$(RM) $(OBJS) $(RES) $(TARGET)

.PHONY: all clean
