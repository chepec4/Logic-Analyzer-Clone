# -----------------------------------------------------------------------------
# C4 ANALYZER - MAKEFILE (V5.0 - Extreme Infrastructure Fix)
# Autor: C4 Productions
# -----------------------------------------------------------------------------

CXX      := g++

# -----------------------------------------------------------------------------
# FLAGS DE COMPILACIÓN (Configuración para entornos de alta complejidad)
# -----------------------------------------------------------------------------
# -O2: Optimización de rendimiento balanceada.
# -std=gnu++14: Estándar C++14 con extensiones de GNU para compatibilidad.
# -fpermissive: Ignora errores de conformidad estricta en código legacy.
# -w: Silencia advertencias para procesar logs de error limpios.
# -msse -msse2: Habilita el conjunto de instrucciones del procesador para sp.h.
# -ftemplate-depth=10000: [CRÍTICO] Límite extremo para plantillas recursivas de Kali.
# -Wno-narrowing: Permite conversiones de tipos que pierden precisión sin fallar.
# -static-libgcc -static-libstdc++: Embebe las librerías para que el DLL sea portable.
# -----------------------------------------------------------------------------
CXXFLAGS := -O2 -std=gnu++14 -fpermissive -w -msse -msse2 \
            -ftemplate-depth=10000 \
            -Wno-narrowing \
            -static-libgcc -static-libstdc++ \
            -DNDEBUG -DWINDOWS_=1 -D__GTHREADS \
            -D_WIN32_WINNT=0x0600 

# -----------------------------------------------------------------------------
# RUTAS DE INCLUSIÓN (Mapa de dependencias)
# -----------------------------------------------------------------------------
INC      := -I. \
            -I../libraries \
            -I../libraries/win \
            -I../libraries/sp \
            -I../libraries/vst \
            -I../libraries/vst/public.sdk/source/vst2.x \
            -I../libraries/vst/pluginterfaces/vst2.x

# -----------------------------------------------------------------------------
# LIBRERÍAS DEL SISTEMA (Enlazado de Windows)
# -----------------------------------------------------------------------------
LIBS     := -lgdi32 -luser32 -lkernel32 -lcomdlg32 -lopengl32 -lglu32 -lwinmm -lole32 -luuid

# -----------------------------------------------------------------------------
# ARCHIVOS FUENTE Y OBJETIVO FINAL
# -----------------------------------------------------------------------------
SRCS     := main.cpp vstsdk-wrapper.cpp
OBJS     := $(SRCS:.cpp=.o)
RES      := main.res
TARGET   := ../C4Analyzer.dll

# -----------------------------------------------------------------------------
# REGLAS DE CONSTRUCCIÓN
# -----------------------------------------------------------------------------

all: $(TARGET)

# 1. Enlazado final del VST DLL
$(TARGET): $(OBJS) $(RES)
	@echo [LINK] Ensamblando binario final C4 Analyzer...
	$(CXX) -shared -s -o $@ $(OBJS) $(RES) $(LIBS) -Wl,--kill-at,--subsystem,windows

# 2. Compilación de los módulos C++
%.o: %.cpp
	@echo [CXX] Procesando modulo: $<...
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

# 3. Compilación de Recursos (Iconos, Versión y Metadatos)
$(RES): rc/main.rc
	@echo [RC] Integrando recursos del sistema...
	windres $(INC) -Irc -i $< -O coff -o $@

# 4. Limpieza del entorno de compilación
clean:
	rm -f $(OBJS) $(RES) $(TARGET)

.PHONY: all clean
