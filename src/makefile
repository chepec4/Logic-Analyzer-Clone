# ================================
# Toolchain
# ================================
CXX     = g++
CC      = gcc
WINDRES = windres

# ================================
# Paths (IMPORTANTE)
# Apuntamos al PADRE de "kali/"
# ================================
INC = -I. \
      -I../libraries \
      -I../libraries/win \
      -I../libraries/vst \
      -I../libraries/vst/public.sdk/source/vst2.x

# ================================
# Flags de compilación (32-bit VST)
# ================================
CXXFLAGS = -O2 -m32 -fpermissive $(INC) \
           -D_WIN32_WINNT=0x0600 \
           -DWIN32 -D_WINDOWS \
           -w

LDFLAGS = -shared -m32 \
          -static-libgcc -static-libstdc++ \
          -Wl,--kill-at

LIBS = -lgdi32 -luser32 -lcomdlg32 -lopengl32 -lwinmm

# ================================
# Files
# ================================
OBJS = vstsdk-wrapper.o sp.o main.res
TARGET = LogicAnalyzer.dll

# ================================
# Build
# ================================
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

# ================================
# Objects
# ================================
vstsdk-wrapper.o: vstsdk-wrapper.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

sp.o: sp.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

main.res: main.rc
	$(WINDRES) $< -O coff -o $@

# ================================
# Debug (para GitHub Actions)
# ================================
debug:
	@echo "==== libraries tree ===="
	ls -R ../libraries

# ================================
# Clean
# ================================
clean:
	rm -f *.o *.res *.dll
