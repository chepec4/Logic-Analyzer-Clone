# Makefile para C4 Analyzer VST
# Programador: ChepeC4
# Company: C4 Productions

CXX      := g++
CXXFLAGS := -O2 -m32 -std=gnu++03 -fpermissive -w -D_WIN32_WINNT=0x0600

# Rutas de inclusión
INC      := -I. \
            -I../libraries \
            -I../libraries/win \
            -I../libraries/vst \
            -I../libraries/vst/public.sdk/source/vst2.x \
            -I../libraries/vst/pluginterfaces/vst2.x \
            -I../libraries/sp

LIBS     := -lgdi32 -luser32 -lcomdlg32 -lopengl32 -lwinmm

OBJS     := vstsdk-wrapper.o sp.o main.res

# CAMBIO IMPORTANTE: Ahora el objetivo es C4Analyzer.dll
all: C4Analyzer.dll

C4Analyzer.dll: $(OBJS)
	$(CXX) -shared -static-libgcc -static-libstdc++ -m32 -Wl,--kill-at -o $@ $(OBJS) $(LIBS)

vstsdk-wrapper.o: vstsdk-wrapper.cpp
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

sp.o: sp.cpp
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

main.res: main.rc
	windres main.rc -O coff -o main.res

clean:
	rm -f *.o *.res *.dll
