# -----------------------------------------------------------------------------
# C4 ANALYZER - MAKEFILE (V11.0 - Definitive Toolchain)
# -----------------------------------------------------------------------------

# Toolchain definitions
CROSS_PREFIX ?= x86_64-w64-mingw32
CXX          := $(CROSS_PREFIX)-g++
WINDRES      := $(CROSS_PREFIX)-windres
RM           := rm -f

# Target
TARGET       := ../C4Analyzer.dll
DEF_FILE     := main.def

# Sources
SRCS         := main.cpp vstsdk-wrapper.cpp
OBJS         := $(SRCS:.cpp=.o)
RES          := main.res

# Build Profile (release by default)
BUILD ?= release

# Compiler Flags
# -w: Suppress warnings (necesario por el VST SDK legacy)
# -msse...: Optimizaciones SIMD requeridas por libraries/sp
COMMON_FLAGS := -m64 -std=gnu++17 -fpermissive -w \
                -msse -msse2 -msse3 -mstackrealign \
                -ftemplate-depth=2000 \
                -DWIN32 -DWIN64 -DWINDOWS_=1 -D_WIN32_WINNT=0x0600 -D__GTHREADS

# Linker Flags
LDFLAGS_COMMON := -m64 -shared -static-libgcc -static-libstdc++ \
                  -Wl,--kill-at,--subsystem,windows,--add-stdcall-alias

ifeq ($(BUILD),debug)
    CXXFLAGS := $(COMMON_FLAGS) -O0 -g3 -D_DEBUG
    LDFLAGS  := $(LDFLAGS_COMMON)
else
    CXXFLAGS := $(COMMON_FLAGS) -O3 -ffast-math -DNDEBUG
    # -s: Strip symbols for smaller binary
    LDFLAGS  := $(LDFLAGS_COMMON) -s
endif

# Include Paths
# Apuntan a la estructura corregida en el paso anterior
INC := -I. -I../libraries -I../libraries/win -I../libraries/sp -I../libraries/vst \
       -I../libraries/vst/public.sdk/source/vst2.x -I../libraries/vst/pluginterfaces/vst2.x

# Libraries [C4 FIX: Added opengl32 and glu32]
LIBS := -lgdi32 -luser32 -lkernel32 -lcomctl32 -lcomdlg32 -lwinmm -lopengl32 -lglu32

# -----------------------------------------------------------------------------
# Rules
# -----------------------------------------------------------------------------

all: $(TARGET)

$(TARGET): $(OBJS) $(RES)
	@echo [LINK] $@
	@$(CXX) $(OBJS) $(RES) $(DEF_FILE) -o $@ $(LDFLAGS) $(LIBS)

%.o: %.cpp
	@echo [CXX] $<
	@$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

$(RES): main.rc
	@echo [RC] $<
	@$(WINDRES) -I. $(INC) -i $< -o $@

clean:
	@echo [CLEAN]
	@$(RM) $(OBJS) $(RES) $(TARGET)

.PHONY: all clean
