# ======================================
# Makefile estable para VST antiguo (SSE)
# ======================================

CXX     = g++
CC      = gcc
WINDRES = windres

# --------------------------------------
# Includes
# IMPORTANTE: apuntar al PADRE de kali/
# --------------------------------------
INC = -I. \
      -I../libraries \
      -I../libraries/win \
      -I../libraries/vst \
      -I../libraries/vst/public.sdk/source/vst2.x \
      -I../libraries/vst/pluginterfaces/vst2.x \
      -I../libraries/sp

# --------------------------------------
# Flags de compilación
# - gnu++03: CLAVE para código viejo
# - sse/sse2: lo que realmente usa el DSP
# --------------------------------------
CXXFLAGS = -O2 -m32 -std=gnu++03 -fpermissive $(INC) \
           -D_WIN32_WINNT=0x0600 \
           -DWIN32 -D_WINDOWS \
           -msse -msse2 \
           -w

# --------------------------------------
# Linker
# --------------------------------------
LDFLAGS = -shared -m32 \
          -static-libgcc -static-libstdc++ \
          -Wl,--kill-at

LIBS = -lgdi32 -luser32 -lcomdlg32 -lopengl32 -lwinmm

# --------------------------------------
# Archivos
# --------------------------------------
OBJS   = vstsdk-wrapper.o sp.o main.res
TARGET = LogicAnalyzer.dll

# --------------------------------------
# Build
# --------------------------------------
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

# --------------------------------------
# Objetos
# --------------------------------------
vstsdk-wrapper.o: vstsdk-wrapper.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

sp.o: sp.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

main.res: main.rc
	$(WINDRES) $< -O coff -o $@

# --------------------------------------
# Debug (útil en GitHub Actions)
# --------------------------------------
debug:
	@echo "==== libraries tree ===="
	ls -R ../libraries

# --------------------------------------
# Clean
# --------------------------------------
clean:
	rm -f *.o *.res *.dll
