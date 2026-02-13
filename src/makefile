# Makefile Maestro C4 - Version Final Blindada
CXX = g++
CC = gcc
WINDRES = windres

# Rutas optimizadas segun tu nueva estructura de carpetas
INC = -I. \
      -I../libraries \
      -I../libraries/win \
      -I../libraries/vst \
      -I../libraries/vst/public.sdk/source/vst2.x \
      -I../libraries/vst/pluginterfaces/vst2.x \
      -I../libraries/sp

# Flags de compilacion para 32 bits
CXXFLAGS = -O2 -m32 -fpermissive $(INC) -D_WIN32_WINNT=0x0600 -w
LDFLAGS = -static-libgcc -static-libstdc++ -shared -m32 -Wl,--kill-at
LIBS = -lgdi32 -luser32 -lcomdlg32 -lopengl32 -lwinmm

# Objetos necesarios para el DLL
OBJS = vstsdk-wrapper.o sp.o main.res

all: LogicAnalyzer.dll

LogicAnalyzer.dll: $(OBJS)
	$(CXX) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

# Compilacion de archivos fuente
vstsdk-wrapper.o: vstsdk-wrapper.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

sp.o: sp.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

main.res: main.rc
	$(WINDRES) $< -O coff -o $@

clean:
	rm -f *.o *.res *.dll
