# -----------------------------------------------------------------------------
# C4 ANALYZER - MAKEFILE (MinGW / GitHub Actions)
# Arquitectura: x64 | Estándar: gnu++03 | Rutas: libraries/
# -----------------------------------------------------------------------------

# 1. Definición del Compilador
CXX      := g++
# Flags críticos:
# -std=gnu++03: Mantiene compatibilidad con el código legacy de Seven Phases
# -static-lib...: Empaqueta las librerías para que funcione en cualquier Windows
# -DWINDOWS_: Define la plataforma para Kali
CXXFLAGS := -O3 -std=gnu++03 -Wall -Wno-narrowing -fpermissive \
            -static-libgcc -static-libstdc++ \
            -DNDEBUG -DWINDOWS_=1 -D__GTHREADS

# 2. Rutas de Inclusión (Las Reglas de Oro)
# Incluimos explícitamente 'win' y 'sp' para evitar errores de "No such file"
INC      := -I. -I../libraries -I../libraries/win -I../libraries/sp

# 3. Librerías del Sistema (Linker)
LIBS     := -lgdi32 -luser32 -lkernel32 -lcomdlg32 -lopengl32 -lglu32 -lwinmm -lole32 -luuid

# 4. Archivos Fuente
SRCS     := main.cpp vstsdk-wrapper.cpp
OBJS     := $(SRCS:.cpp=.o)
RES      := main.res
# Nombre final del binario
TARGET   := ../C4Analyzer.dll

# 5. Reglas de Construcción
all: $(TARGET)

# Linker: Crea la DLL final
$(TARGET): $(OBJS) $(RES)
	@echo [LINK] Generando C4 Analyzer DLL...
	$(CXX) -shared -s -o $@ $(OBJS) $(RES) $(LIBS) -Wl,--kill-at,--subsystem,windows

# Compilador C++
%.o: %.cpp
	@echo [CXX] Compilando $<...
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

# Compilador de Recursos (Iconos y Version.h)
$(RES): rc/main.rc
	@echo [RC] Procesando recursos...
	windres $(INC) -Irc -i $< -O coff -o $@

# Limpieza
clean:
	rm -f $(OBJS) $(RES) $(TARGET)

.PHONY: all clean
