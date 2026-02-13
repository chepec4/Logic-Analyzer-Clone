# Makefile de Supervivencia para chepeC4
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

# FLAGS CLAVE:
# -fpermissive: Ignora errores de estilo antiguo
# -msse -msse2 -msse3: Activa las instrucciones de audio que fallan
# -Dmm4=sp::m4f: Arregla el error de 'mm4 was not declared'
CXXFLAGS = -O2 -m32 -std=c++11 -fpermissive $(INC) \
           -D_WIN32_WINNT=0x0600 -msse -msse2 -msse3 \
           -Dmm4="sp::m4f" -w

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
