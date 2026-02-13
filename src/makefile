# Makefile Blindado para chepeC4 - Correccion de SSE
CXX = g++
CC = gcc
WINDRES = windres

# Rutas validadas
INC = -I. \
      -I../libraries \
      -I../libraries/win \
      -I../libraries/vst \
      -I../libraries/vst/public.sdk/source/vst2.x \
      -I../libraries/vst/pluginterfaces/vst2.x \
      -I../libraries/sp

# CAMBIO CLAVE: Agregamos -fpermissive y forzamos el estándar C++11 
# para que acepte los operadores de __m128
CXXFLAGS = -O2 -m32 -std=c++11 -fpermissive $(INC) -D_WIN32_WINNT=0x0600 -msse -msse2 -w
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
