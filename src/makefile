# Makefile Simplificado para chepeC4 - Logica de Carpetas Padre
CXX = g++
CC = gcc
WINDRES = windres

# Apuntamos a los padres para que los includes tipo "kali/archivo.h" funcionen
INC = -I. \
      -I../libraries \
      -I../libraries/win \
      -I../libraries/vst \
      -I../libraries/sp

# Flags de compilacion para 32 bits
CXXFLAGS = -O2 -m32 -fpermissive $(INC) -D_WIN32_WINNT=0x0600 -w
LDFLAGS = -static-libgcc -static-libstdc++ -shared -m32 -Wl,--kill-at
LIBS = -lgdi32 -luser32 -lcomdlg32 -lopengl32 -lwinmm

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

# Comando de debug solicitado para ver la estructura real en GitHub Actions
debug:
	ls -R ../libraries
