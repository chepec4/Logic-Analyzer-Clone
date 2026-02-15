# -----------------------------------------------------------------------------
# C4 ANALYZER - MAKEFILE (MinGW / GitHub Actions)
# -----------------------------------------------------------------------------

CXX      := g++
CXXFLAGS := -O3 -std=gnu++03 -Wall -Wno-narrowing -fpermissive -Wno-unknown-pragmas \
            -static-libgcc -static-libstdc++ \
            -DNDEBUG -DWINDOWS_=1 -D__GTHREADS

# -----------------------------------------------------------------------------
# RUTAS DE INCLUSIÓN CORREGIDAS
# -----------------------------------------------------------------------------
# Basado en tu información, las cabeceras están en estas rutas de tu repo:
INC      := -I. \
            -I../libraries \
            -I../libraries/win \
            -I../libraries/sp \
            -I../libraries/vst \
            -I../libraries/vst/public.sdk/source/vst2.x \
            -I../libraries/vst/pluginterfaces/vst2.x

LIBS     := -lgdi32 -luser32 -lkernel32 -lcomdlg32 -lopengl32 -lglu32 -lwinmm -lole32 -luuid

SRCS     := main.cpp vstsdk-wrapper.cpp
OBJS     := $(SRCS:.cpp=.o)
RES      := main.res
TARGET   := ../C4Analyzer.dll

all: $(TARGET)

$(TARGET): $(OBJS) $(RES)
	@echo [LINK] Creando $(TARGET)...
	$(CXX) -shared -s -o $@ $(OBJS) $(RES) $(LIBS) -Wl,--kill-at,--subsystem,windows

%.o: %.cpp
	@echo [CXX] Compilando $<...
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

$(RES): rc/main.rc
	@echo [RC] Compilando recursos...
	windres $(INC) -Irc -i $< -O coff -o $@

clean:
	rm -f $(OBJS) $(RES) $(TARGET)

.PHONY: all clean
