# =============================================================================
# MAKEFILE FINAL C4 ANALYZER (Soporte SSE Forzado + 32-bit Legacy Fix)
# Programador: ChepeC4
# =============================================================================

CXX      := g++

# FLAGS CRÍTICAS:
# -m32: Fuerza arquitectura de 32 bits (vital para libs antiguas)
# -msse -msse2 -msse3: Habilita instrucciones matemáticas para audio
# -fpermissive: Convierte errores de código antiguo en simples advertencias
# -w: Silencia el ruido en el log
CXXFLAGS := -O2 -m32 -msse -msse2 -msse3 -std=gnu++03 -fpermissive -w -D_WIN32_WINNT=0x0600 -DBUILDING_VST=1

# Rutas de inclusión
INC      := -I. \
            -I../libraries \
            -I../libraries/win \
            -I../libraries/vst \
            -I../libraries/vst/public.sdk/source/vst2.x \
            -I../libraries/vst/pluginterfaces/vst2.x \
            -I../libraries/sp

# Librerías estáticas para que funcione en cualquier Windows sin instalar nada
LIBS     := -static-libgcc -static-libstdc++ -lgdi32 -luser32 -lcomdlg32 -lopengl32 -lwinmm

# Objetos a compilar
OBJS     := vstsdk-wrapper.o sp.o main.res

# Objetivo Principal
all: C4Analyzer.dll

# Linker (Creación del DLL)
# --export-all-symbols: Hace visible todo el código al DAW
# --kill-at: Limpia los nombres de funciones para que FL Studio las reconozca
C4Analyzer.dll: $(OBJS)
	$(CXX) -shared -m32 -o $@ $(OBJS) $(LIBS) -Wl,--export-all-symbols -Wl,--kill-at

# Compilación de C++
vstsdk-wrapper.o: vstsdk-wrapper.cpp
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

sp.o: sp.cpp
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

# Compilación de Recursos (Iconos, Menús)
main.res: main.rc
	windres main.rc -O coff -o main.res

# Limpieza
clean:
	rm -f *.o *.res *.dll
