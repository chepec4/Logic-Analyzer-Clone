# -----------------------------------------------------------------------------
# C4 ANALYZER - MAKEFILE (V10.0 - Definitive Production x64)
# Optimizado para: Windows Server 2025 | MinGW-w64 (GCC 12.2) | C++17
# -----------------------------------------------------------------------------

CXX      := g++
WINDRES  := windres
RM       := rm -f

# -----------------------------------------------------------------------------
# ARQUITECTURA Y RENDIMIENTO (Regla de Oro: SIMD + Estabilidad)
# -----------------------------------------------------------------------------
# -O3 + -ffast-math: Rendimiento crítico para el banco de filtros.
# -std=gnu++17: Requerido por 'if constexpr' en analyzer.h.
# -fpermissive: Vital para la compatibilidad con la librería Kali (Seven Phases).
# -----------------------------------------------------------------------------
CXXFLAGS := -m64 -O3 -ffast-math -std=gnu++17 -fpermissive -w \
            -msse -msse2 -msse3 -mstackrealign \
            -ftemplate-depth=2000 \
            -static-libgcc -static-libstdc++ \
            -DNDEBUG -DWINDOWS_=1 -D_WIN32_WINNT=0x0600 -D__GTHREADS

# -----------------------------------------------------------------------------
# RUTAS DE INCLUSIÓN
# -----------------------------------------------------------------------------
INC      := -I. \
            -I../libraries \
            -I../libraries/win \
            -I../libraries/sp \
            -I../libraries/vst \
            -I../libraries/vst/public.sdk/source/vst2.x \
            -I../libraries/vst/pluginterfaces/vst2.x

# -----------------------------------------------------------------------------
# LIBRERÍAS Y ENLAZADOR (VST Compliance)
# -----------------------------------------------------------------------------
# --kill-at: Asegura que el DAW encuentre VSTPluginMain sin decoración de nombres.
# --add-stdcall-alias: Compatibilidad máxima con Ableton, Cubase y Logic.
# -----------------------------------------------------------------------------
LIBS     := -lgdi32 -luser32 -lkernel32 -lcomctl32 -lcomdlg32 -lwinmm -lole32 -luuid -loleaut32 -lopengl32
LDFLAGS  := -m64 -shared -s -Wl,--kill-at -Wl,--subsystem,windows -Wl,--add-stdcall-alias

# -----------------------------------------------------------------------------
# ARCHIVOS DEL PROYECTO
# -----------------------------------------------------------------------------
SRCS     := main.cpp vstsdk-wrapper.cpp
OBJS     := $(SRCS:.cpp=.o)
RES      := main.res
TARGET   := ../C4Analyzer.dll

# -----------------------------------------------------------------------------
# REGLAS DE CONSTRUCCIÓN
# -----------------------------------------------------------------------------

all: $(TARGET)



# [LINKER] Generación del binario VST DLL
$(TARGET): $(OBJS) $(RES)
	@echo [LINKER] Ensamblando binario de producción: $@
	$(CXX) $(LDFLAGS) -o $@ $(OBJS) $(RES) $(LIBS)

# [CXX] Compilación de módulos C++
%.o: %.cpp
	@echo [CXX] Procesando núcleo: $<
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

# [RC] Compilación de Recursos (Iconos de Logic, Versión, Manifiesto)
$(RES): rc/main.rc
	@echo [RC] Compilando recursos visuales: $<
	$(WINDRES) $(INC) -Irc -i $< -O coff -o $@

# Limpieza profunda para regenerar desde cero
clean:
	@echo [CLEAN] Eliminando objetos y binarios corruptos...
	$(RM) $(OBJS) $(RES) $(TARGET)

.PHONY: all clean
