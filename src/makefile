# -----------------------------------------------------------------------------
# C4 ANALYZER - MAKEFILE (V4.0 - Template Depth Fix)
# Autor: C4 Productions
# -----------------------------------------------------------------------------

CXX      := g++

# -----------------------------------------------------------------------------
# FLAGS DE COMPILACIÓN
# -----------------------------------------------------------------------------
# -O2: Optimización de velocidad estándar.
# -std=gnu++14: Estándar C++14 con extensiones GNU.
# -fpermissive: Permite código legacy que no cumple estrictamente el estándar.
# -w: Desactiva todos los warnings para limpiar la salida.
# -msse -msse2: Habilita instrucciones SIMD (requerido por sp.h).
# -ftemplate-depth=5000: [CRÍTICO] Aumenta el límite de recursión para Kali.
# -Wno-narrowing: Evita errores de conversión de tipos en listas de inicialización.
# -static-lib...: Enlazado estático para portabilidad (no requiere DLLs extra).
# -----------------------------------------------------------------------------
CXXFLAGS := -O2 -std=gnu++14 -fpermissive -w -msse -msse2 \
            -ftemplate-depth=5000 \
            -Wno-narrowing \
            -static-libgcc -static-libstdc++ \
            -DNDEBUG -DWINDOWS_=1 -D__GTHREADS \
            -D_WIN32_WINNT=0x0600 

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
# -Wl,--kill-at: Obligatorio para exportar VSTPluginMain correctamente
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
