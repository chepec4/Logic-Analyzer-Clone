# -----------------------------------------------------------------------------
# C4 ANALYZER - MAKEFILE (MinGW / GitHub Actions)
# Protocolo: gnu++03 | Static Linking
# -----------------------------------------------------------------------------

# Compilador y Flags
CXX      := g++
# Flags:
# -Wno-unknown-pragmas: Silencia los warnings de #pragma de Visual Studio
# -fpermissive: Permite código legacy un poco más relajado
CXXFLAGS := -O3 -std=gnu++03 -Wall -Wno-narrowing -fpermissive -Wno-unknown-pragmas \
            -static-libgcc -static-libstdc++ \
            -DNDEBUG -DWINDOWS_=1 -D__GTHREADS

# -----------------------------------------------------------------------------
# RUTAS DE INCLUSIÓN (CRÍTICO)
# -----------------------------------------------------------------------------
# Aquí agregamos las rutas profundas del VST SDK para encontrar audioeffectx.h
INC      := -I. \
            -I../libraries \
            -I../libraries/win \
            -I../libraries/sp \
            -I../libraries/vst \
            -I../libraries/vst/public.sdk/source/vst2.x \
            -I../libraries/vst/pluginterfaces/vst2.x \
            -I../libraries/vst/vstsdk2.4/public.sdk/source/vst2.x \
            -I../libraries/vst/vstsdk2.4/pluginterfaces/vst2.x

# Librerías del Sistema (Linker)
LIBS     := -lgdi32 -luser32 -lkernel32 -lcomdlg32 -lopengl32 -lglu32 -lwinmm -lole32 -luuid

# Archivos Fuente
SRCS     := main.cpp vstsdk-wrapper.cpp
OBJS     := $(SRCS:.cpp=.o)
RES      := main.res
TARGET   := ../C4Analyzer.dll

# Regla Principal
all: $(TARGET)

# Linker (Creación del DLL)
$(TARGET): $(OBJS) $(RES)
	@echo [LINK] Creando $(TARGET)...
	$(CXX) -shared -s -o $@ $(OBJS) $(RES) $(LIBS) -Wl,--kill-at,--subsystem,windows

# Compilación de C++
%.o: %.cpp
	@echo [CXX] Compilando $<...
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

# Compilación de Recursos
$(RES): rc/main.rc
	@echo [RC] Compilando recursos...
	windres $(INC) -Irc -i $< -O coff -o $@

# Limpieza
clean:
	rm -f $(OBJS) $(RES) $(TARGET)

.PHONY: all clean
