# -----------------------------------------------------------------------------
# C4 ANALYZER - MAKEFILE (V6.0 - Final Production Build)
# Optimizado para MinGW-w64 / GCC 12.2+
# -----------------------------------------------------------------------------

CXX      := g++
RM       := rm -f

# -----------------------------------------------------------------------------
# FLAGS DE COMPILACIÓN (Rendimiento Máximo y Estabilidad)
# -----------------------------------------------------------------------------
# -O3: Optimización agresiva para procesamiento de audio.
# -ffast-math: Acelera cálculos matemáticos (especial para el motor SIMD sp.h).
# -std=gnu++14: Estándar base compatible con la infraestructura Kali.
# -ftemplate-depth=1000: Nivel seguro tras corregir la recursión en runtime.h.
# -msse -msse2 -msse3: Habilita instrucciones vectoriales modernas.
# -----------------------------------------------------------------------------
CXXFLAGS := -O3 -ffast-math -std=gnu++14 -fpermissive -w \
            -msse -msse2 -msse3 \
            -ftemplate-depth=1000 \
            -Wno-narrowing \
            -static-libgcc -static-libstdc++ \
            -DNDEBUG -DWINDOWS_=1 -D_WIN32_WINNT=0x0600 \
            -D__GTHREADS

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
# LIBRERÍAS Y ENLAZADO
# -----------------------------------------------------------------------------
LIBS     := -lgdi32 -luser32 -lkernel32 -lcomdlg32 -lopengl32 -lglu32 -lwinmm -lole32 -luuid
LDFLAGS  := -shared -s -Wl,--kill-at -Wl,--subsystem,windows

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

# [LINK] Ensamblado del DLL Final
$(TARGET): $(OBJS) $(RES)
	@echo [LINKER] Generando binario VST: $@
	$(CXX) $(LDFLAGS) -o $@ $(OBJS) $(RES) $(LIBS)

# [CXX] Compilación de Código Fuente
%.o: %.cpp
	@echo [CXX] Compilando modulo: $<
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

# [RC] Compilación de Recursos de Windows
$(RES): rc/main.rc
	@echo [RC] Procesando recursos e iconos: $<
	windres $(INC) -Irc -i $< -O coff -o $@

# Limpieza del Directorio
clean:
	@echo [CLEAN] Eliminando archivos temporales...
	$(RM) $(OBJS) $(RES) $(TARGET)

.PHONY: all clean
