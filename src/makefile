# =============================================================================
# MAKEFILE BOOTSTRAPPER V12.0 (Generador Automatico)
# Autor: ChepeC4 - Finalizacion de Namespaces (Thunk & DrawThunk)
# =============================================================================

all:
	@echo "--- Generando Makefile corregido (V12.0) ---"
	@echo "CXX := g++" > Makefile.run
	@echo "CXXFLAGS := -O2 -m32 -msse -msse2 -msse3 -std=gnu++98 -fpermissive -fno-access-control -w -Wno-narrowing -Wno-write-strings -ftemplate-depth=3000 -D_WIN32_WINNT=0x0600 -DBUILDING_VST=1 -DNAME=\"\\\"C4 Analyzer\\\"\" -DCOMPANY=\"\\\"C4 Productions\\\"\" -DAUTHOR=\"\\\"ChepeC4\\\"\"" >> Makefile.run
	@echo "INC := -I. -I../libraries -I../libraries/win -I../libraries/vst -I../libraries/vst/public.sdk/source/vst2.x -I../libraries/vst/pluginterfaces/vst2.x -I../libraries/sp" >> Makefile.run
	@echo "LIBS := -static-libgcc -static-libstdc++ -lgdi32 -luser32 -lcomdlg32 -lopengl32 -lwinmm -lole32 -lshlwapi" >> Makefile.run
	@echo "OBJS := vstsdk-wrapper.o sp.o main.res" >> Makefile.run
	@echo "TARGET := C4Analyzer.dll" >> Makefile.run
	@echo "VPATH := ../libraries/sp" >> Makefile.run
	@echo "" >> Makefile.run
	@echo "all: fix-libraries \$$(TARGET)" >> Makefile.run
	@echo "" >> Makefile.run
	@echo "fix-libraries:" >> Makefile.run
	@echo "	@echo \"--- [FASE 1] Reparaciones Estructurales ---\"" >> Makefile.run
	@echo "	-grep -q \"typedef struct HINSTANCE__\" ../libraries/kali/app.h || sed -i 's/typedef HINSTANCE__ Module;/typedef struct HINSTANCE__ Module;/g' ../libraries/kali/app.h" >> Makefile.run
	@echo "	-grep -q \"((char*&)\" ../libraries/kali/runtime.h || sed -i 's/\*dst++= \*src++;/*((char*\\&)dst)++ = *((const char*\\&)src)++;/g' ../libraries/kali/runtime.h" >> Makefile.run
	@echo "	-grep -q \"string(\\\"%s\\\"\" ../libraries/win/kali/window.h || sed -i 's/!comments ? text/!comments ? string(\"%s\", text)/g' ../libraries/win/kali/window.h" >> Makefile.run
	@echo "	-grep -q \"//#define DBG\" ../libraries/kali/dbgutils.h || sed -i 's/#define DBG (defined(_DEBUG))/\\/\\/#define DBG (defined(_DEBUG))/g' ../libraries/kali/dbgutils.h" >> Makefile.run
	@echo "	@echo \"--- [FASE 2] Reparaciones de Templates ---\"" >> Makefile.run
	@echo "	-
