# Makefile Maestro - LogicAnalyzer.dll
# Basado en la arquitectura Seven Phases y VST SDK 2.4

CXX      := g++
# Flags críticos:
# -m32: Obligatorio para VST 2.4 antiguo.
# -shared: Genera DLL, no EXE.
# -fpermissive: Permite código legacy de Kali/SP.
CXXFLAGS := -O2 -m32 -std=gnu++03 -fpermissive -w -D_WIN32_WINNT=0x0600

# Rutas extraídas de tu estructura de carpetas
INC      := -I. \
            -I../libraries \
            -I../libraries/win \
            -I../libraries/vst \
            -I../libraries/vst/public.sdk/source/vst2.x \
            -I../libraries/vst/pluginterfaces/vst2.x \
            -I../libraries/sp

# Librerías de sistema que Kali necesita para dibujar en Windows
LIBS     := -lgdi32 -luser32 -lcomdlg32 -lopengl32 -lwinmm

# Objetos a compilar
OBJS     := vstsdk-wrapper.o sp.o main.res

# Meta: Crear el archivo final
all: LogicAnalyzer.dll

LogicAnalyzer.dll: $(OBJS)
	$(CXX) -shared -static-libgcc -static-libstdc++ -m32 -Wl,--kill-at -o $@ $(OBJS) $(LIBS)

# Reglas de compilación individual
vstsdk-wrapper.o: vstsdk-wrapper.cpp
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

sp.o: sp.cpp
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

main.res: main.rc
	windres $< -O coff -o $@

clean:
	rm -f *.o *.res *.dll
