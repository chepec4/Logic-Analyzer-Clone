# Makefile Definitivo para chepeC4 - Optimizado con rutas reales
CXX = g++
CC = gcc
WINDRES = windres

# Rutas exactas segun tus capturas de pantalla
INC = -I. \
      -I../libraries \
      -I../libraries/kali \
      -I../libraries/win/kali \
      -I../libraries/win \
      -I../libraries/vst

# Flags de compilacion
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

clean:
	rm -f *.o *.res *.dll
