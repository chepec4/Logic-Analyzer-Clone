# Makefile optimizado para C4 - Logic Analyzer
CXX = g++
CC = gcc
WINDRES = windres

# Rutas de librerias y headers (basado en tu estructura de carpetas)
INC = -I. -I../libraries -I../libraries/kali -I../libraries/vstsdk2.4

# Opciones de compilacion para 32 bits
CXXFLAGS = -O2 -m32 -fpermissive $(INC) -D_WIN32_WINNT=0x0600
LDFLAGS = -static-libgcc -static-libstdc++ -shared -m32 -Wl,--kill-at
LIBS = -lgdi32 -luser32 -lcomdlg32 -lopengl32 -lwinmm

# Archivos a compilar (basado en tus fotos de la carpeta src)
OBJS = vstsdk-wrapper.o sp.o main.res

all: LogicAnalyzer.dll

LogicAnalyzer.dll: $(OBJS)
	$(CXX) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

vstsdk-wrapper.o: vstsdk-wrapper.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

sp.o: sp.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

main.res: main.rc
	$(WINDRES) $< -O coff -o $@

clean:
	rm -f *.o *.res *.dll
