# -----------------------------------------------------------------------------
# C4 ANALYZER - MAKEFILE (MinGW-w64 x64 / GitHub Actions friendly)
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

# Build profile: release | debug
BUILD ?= release

COMMON_FLAGS := -m64 -std=gnu++17 -fpermissive -w \
                -msse -msse2 -msse3 -mstackrealign \
                -ftemplate-depth=2000 \
                -DWIN32 -DWIN64 -DWINDOWS_=1 -D_WIN32_WINNT=0x0600 -D__GTHREADS

ifeq ($(BUILD),debug)
CXXFLAGS := $(COMMON_FLAGS) -O0 -g3 -D_DEBUG
LDFLAGS  := -m64 -shared -Wl,--kill-at -Wl,--subsystem,windows -Wl,--add-stdcall-alias
else
CXXFLAGS := $(COMMON_FLAGS) -O3 -ffast-math -DNDEBUG -static-libgcc -static-libstdc++
LDFLAGS  := -m64 -shared -s -Wl,--kill-at -Wl,--subsystem,windows -Wl,--add-stdcall-alias
endif

INC      := -I. \
            -I../libraries \
            -I../libraries/win \
            -I../libraries/sp \
            -I../libraries/vst \
            -I../libraries/vst/public.sdk/source/vst2.x \
            -I../libraries/vst/pluginterfaces/vst2.x

LIBS     := -lgdi32 -luser32 -lkernel32 -lcomctl32 -lcomdlg32 -lwinmm -lole32 -luuid -loleaut32 -lopengl32

all: check-toolchain $(TARGET)

check-toolchain:
	@command -v $(CXX) >/dev/null 2>&1 || { \
		echo "Error: $(CXX) no está disponible."; \
		echo "GitHub Actions (ubuntu-latest): sudo apt-get update && sudo apt-get install -y mingw-w64"; \
		echo "Instalación: https://www.mingw-w64.org/downloads/"; \
		exit 1; \
	}
	@$(CXX) -dumpmachine | grep -qi '^x86_64-.*mingw' || { \
		echo "Error: $(CXX) no apunta a MinGW-w64 x64 (target esperado: x86_64-*-mingw*)."; \
		echo "Sugerencia: make -C src CROSS_PREFIX=x86_64-w64-mingw32"; \
		exit 1; \
	}
	@command -v $(WINDRES) >/dev/null 2>&1 || { \
		echo "Error: $(WINDRES) no está disponible."; \
		exit 1; \
	}

$(TARGET): $(OBJS) $(RES) $(DEF_FILE)
	@echo [LINKER] Ensamblando DLL: $@
	$(CXX) $(LDFLAGS) -o $@ $(OBJS) $(RES) $(DEF_FILE) $(LIBS)

%.o: %.cpp
	@echo [CXX] Compilando: $<
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

$(RES): rc/main.rc
	@echo [RC] Compilando recursos: $<
	$(WINDRES) $(INC) -Irc -i $< -O coff -o $@

strip: $(TARGET)
	@command -v $(STRIP) >/dev/null 2>&1 && $(STRIP) --strip-unneeded $(TARGET) || true

clean:
	@echo [CLEAN] Limpiando artefactos...
	$(RM) $(OBJS) $(RES) $(TARGET)

.PHONY: all clean check-toolchain strip
