# Makefile Profesional para LogicAnalyzer.dll
CXX      := g++
CXXFLAGS := -O2 -m32 -std=gnu++03 -fpermissive -w
# Rutas de inclusión validadas según tus librerías
INC      := -I. -I../libraries -I../libraries/win -I../libraries/vst \
            -I../libraries/vst/public.sdk/source/vst2.x \
            -I../libraries/vst/pluginterfaces/vst2.x -I../libraries/sp

# Flags de enlace para generar un DLL VST
LDFLAGS  := -shared -static-libgcc -static-libstdc++ -m32 -Wl,--kill-at
LIBS     := -lgdi32 -luser32 -lcomdlg32 -lopengl32 -lwinmm

# Objetos necesarios para el Spectrum Analyzer
OBJS     := vstsdk-wrapper.o sp.o main.res

all: LogicAnalyzer.dll

LogicAnalyzer.dll: $(OBJS)
	$(CXX) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

# Compilación del Wrapper que incluye audioeffect.cpp
vstsdk-wrapper.o: vstsdk-wrapper.cpp
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

sp.o: sp.cpp
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

main.res: main.rc
	windres $< -O coff -o $@

clean:
	rm -f *.o *.res *.dll
