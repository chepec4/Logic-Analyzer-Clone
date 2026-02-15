# -----------------------------------------------------------------------------
# C4 ANALYZER - MAKEFILE (V11.0 - Definitive Toolchain)
# -----------------------------------------------------------------------------

CROSS_PREFIX ?= x86_64-w64-mingw32
CXX          := $(CROSS_PREFIX)-g++
WINDRES      := $(CROSS_PREFIX)-windres
STRIP        := $(CROSS_PREFIX)-strip
RM           := rm -f

TARGET       := ../C4Analyzer.dll
DEF_FILE     := main.def
SRCS         := main.cpp vstsdk-wrapper.cpp
OBJS         := $(SRCS:.cpp=.o)
RES          := main.res

# Perfil por defecto: release
BUILD ?= release

COMMON_FLAGS := -m64 -std=gnu++17 -fpermissive -w \
                -msse -msse2 -msse3 -mstackrealign \
                -ftemplate-depth=2000 \
                -DWIN32 -DWIN64 -DWINDOWS_=1 -D_WIN32_WINNT=0x0600 -D__GTHREADS

ifeq ($(BUILD),debug)
    CXXFLAGS := $(COMMON_FLAGS) -O0 -g3 -D_DEBUG
    LDFLAGS  := -m64 -shared -Wl,--kill-at,--subsystem,windows,--add-stdcall-alias
else
    CXXFLAGS := $(COMMON_FLAGS) -O3 -ffast-math -DNDEBUG -static-libgcc -static-libstdc++
    LDFLAGS  := -m64 -shared -s -Wl,--kill-at,--subsystem,windows,--add-stdcall-alias
endif

INC := -I. -I../libraries -I../libraries/win -I../libraries/sp -I../libraries/vst \
       -I../libraries/vst/public.sdk/source/vst2.x -I../libraries/vst/pluginterfaces/vst2.x

LIBS := -lgdi32 -luser32 -lkernel32 -lcomctl32 -lcomdlg32 -lwinmm -lole32 -luuid -loleaut32 -lopengl32

all: check-toolchain $(TARGET)

check-toolchain:
	@$(CXX) -dumpmachine | grep -qi 'x86_64.*mingw' || (echo "Error: Toolchain no compatible." && exit 1)

$(TARGET): $(OBJS) $(RES) $(DEF_FILE)
	@echo [LINK] Building: $@
	$(CXX) $(LDFLAGS) -o $@ $(OBJS) $(RES) $(DEF_FILE) $(LIBS)

%.o: %.cpp
	@echo [CXX] $<
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

$(RES): rc/main.rc
	@echo [RC] $<
	$(WINDRES) $(INC) -Irc -i $< -O coff -o $@

clean:
	@echo [CLEAN]
	$(RM) $(OBJS) $(RES) $(TARGET)

.PHONY: all clean check-toolchain
