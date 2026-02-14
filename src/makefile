# =============================================================================
# MAKEFILE FINAL - C4 ANALYZER (Estrategia Limpia)
# =============================================================================

CXX      := g++
# Flags críticas: -fpermissive para ignorar errores de tipos en kali
CXXFLAGS := -O2 -m32 -msse -msse2 -msse3 -std=gnu++03 -fpermissive -w -D_WIN32_WINNT=0x0600 -DBUILDING_VST=1

INC      := -I. \
            -I../libraries \
            -I../libraries/win \
            -I../libraries/vst \
            -I../libraries/vst/public.sdk/source/vst2.x \
            -I../libraries/vst/pluginterfaces/vst2.x \
            -I../libraries/sp

LIBS     := -static-libgcc -static-libstdc++ -lgdi32 -luser32 -lcomdlg32 -lopengl32 -lwinmm -lole32 -lshlwapi

OBJS     := vstsdk-wrapper.o sp.o main.res

# REGLA DE PARCHEO MANUAL (Solo analyzer.h necesita esto)
all: patch-analyzer C4Analyzer.dll

patch-analyzer:
	@echo "--- Parchando analyzer.h ---"
	-sed -i 's/Dst::T/typename Dst::T/g' analyzer.h
	@echo "--- Analyzer parchado ---"

C4Analyzer.dll: $(OBJS)
	$(CXX) -shared -m32 -o $@ $(OBJS) $(LIBS) -Wl,--export-all-symbols -Wl,--kill-at

vstsdk-wrapper.o: vstsdk-wrapper.cpp
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

sp.o: sp.cpp
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

main.res: main.rc
	windres main.rc -O coff -o main.res

clean:
	rm -f *.o *.res *.dll
