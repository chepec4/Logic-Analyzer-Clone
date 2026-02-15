# -----------------------------------------------------------------------------
# C4 ANALYZER - MAKEFILE (Modo Compatibilidad Extrema - MinGW/GCC 12)
# Autor: C4 Productions
# -----------------------------------------------------------------------------

CXX      := g++

# -----------------------------------------------------------------------------
# FLAGS DE COMPILACIÓN (La magia para que funcione código de 2008 en 2026)
# -----------------------------------------------------------------------------
# -O2: Optimización equilibrada (Rápido y estable).
# -std=gnu++14: Usamos C++14 para soportar la estructura m128 del Paso C.
# -fpermissive: CRÍTICO. Permite código "sucio" antiguo (ignora errores de typename).
# -w: Silencia todos los warnings para ver si hay errores reales.
# -msse -msse2: Habilita las instrucciones matemáticas del procesador.
# -static-lib...: Mete las librerías dentro del DLL (Portable).
# -----------------------------------------------------------------------------
CXXFLAGS := -O2 -std=gnu++14 -fpermissive -w -msse -msse2 \
            -static-libgcc -static-libstdc++ \
            -DNDEBUG -DWINDOWS_=1 -D__GTHREADS \
            -D_WIN32_WINNT=0x0600 

# -----------------------------------------------------------------------------
# RUTAS DE INCLUSIÓN (Tu mapa de archivos local)
# -----------------------------------------------------------------------------
INC      := -I. \
            -I../libraries \
            -I../libraries/win \
            -I../libraries/sp \
            -I../libraries/vst \
            -I../libraries/vst/public.sdk/source/vst2.x \
            -I../libraries/vst/pluginterfaces/vst2.x

# -----------------------------------------------------------------------------
# LIBRERÍAS DE WINDOWS (Linker)
# -----------------------------------------------------------------------------
LIBS     := -lgdi32 -luser32 -lkernel32 -lcomdlg32 -lopengl32 -lglu32 -lwinmm -lole32 -luuid

# -----------------------------------------------------------------------------
# ARCHIVOS FUENTE Y OBJETIVOS
# -----------------------------------------------------------------------------
SRCS     := main.cpp vstsdk-wrapper.cpp
OBJS     := $(SRCS:.cpp=.o)
RES      := main.res
TARGET   := ../C4Analyzer.dll

# -----------------------------------------------------------------------------
# REGLAS DE CONSTRUCCIÓN
# -----------------------------------------------------------------------------

all: $(TARGET)

# 1. Enlazado final (Creación del DLL)
# -Wl,--kill-at: Obligatorio para que los DAWs encuentren 'VSTPluginMain'
$(TARGET): $(OBJS) $(RES)
	@echo [LINK] Ensamblando C4 Analyzer DLL...
	$(CXX) -shared -s -o $@ $(OBJS) $(RES) $(LIBS) -Wl,--kill-at,--subsystem,windows

# 2. Compilación de C++
%.o: %.cpp
	@echo [CXX] Compilando $<...
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

# 3. Compilación de Recursos (Iconos y Versión)
$(RES): rc/main.rc
	@echo [RC] Integrando recursos e identidad...
	windres $(INC) -Irc -i $< -O coff -o $@

# Limpieza
clean:
	rm -f $(OBJS) $(RES) $(TARGET)

.PHONY: all clean
