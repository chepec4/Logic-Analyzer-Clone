# Makefile C4 Analyzer - Versión 64-bit (x64)
# Programador: ChepeC4

CXX      := g++
CXXFLAGS := -O2 -std=gnu++03 -fpermissive -w -D_WIN32_WINNT=0x0600 -DBUILDING_VST=1

INC      := -I. \
            -I../libraries \
            -I../libraries/win \
            -I../libraries/vst \
            -I../libraries/vst/public.sdk/source/vst2.x \
            -I../libraries/vst/pluginterfaces/vst2.x \
            -I../libraries/sp

LIBS     := -static-libgcc -static-libstdc++ -lgdi32 -luser32 -lcomdlg32 -lopengl32 -lwinmm

OBJS     := vstsdk-wrapper.o sp.o main.res

all: C4Analyzer.dll

C4Analyzer.dll: $(OBJS)
	$(CXX) -shared -o $@ $(OBJS) $(LIBS) -Wl,--export-all-symbols

vstsdk-wrapper.o: vstsdk-wrapper.cpp
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

sp.o: sp.cpp
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

main.res: main.rc
	windres main.rc -O coff -o main.res

clean:
	rm -f *.o *.res *.dll
