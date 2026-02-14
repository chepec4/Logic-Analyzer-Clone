# =============================================================================
# MAKEFILE MAESTRO - C4 ANALYZER
# Autor: ChepeC4 | C4 Productions
# =============================================================================

CXX      := g++
# Se agregan definiciones de identidad para asegurar el branding en compilación
CXXFLAGS := -O2 -m32 -msse -msse2 -msse3 -std=gnu++98 -fpermissive -fno-access-control -w \
            -D_WIN32_WINNT=0x0600 -DBUILDING_VST=1 \
            -DNAME="\"C4 Analyzer\"" \
            -DCOMPANY="\"C4 Productions\"" \
            -DAUTHOR="\"ChepeC4\""

INC      := -I. \
            -I../libraries \
            -I../libraries/win \
            -I../libraries/vst \
            -I../libraries/vst/public.sdk/source/vst2.x \
            -I../libraries/vst/pluginterfaces/vst2.x \
            -I../libraries/sp

# Librerias del sistema necesarias para los parches de Windows (colores y streams)
LIBS     := -static-libgcc -static-libstdc++ \
            -lgdi32 -luser32 -lcomdlg32 -lopengl32 -lwinmm -lole32 -lshlwapi

OBJS     := vstsdk-wrapper.o sp.o main.res

# Nombre del binario final registrado
TARGET   := C4Analyzer.dll

# Objetivo Principal
all: fix-libraries $(TARGET)

# =============================================================================
# ZONA DE PARCHES DE LIBRERIAS (KALI)
# Nota: Ya no se parchea src/ (analyzer o editor) porque el codigo fuente 
# fue corregido manualmente. Solo tocamos las librerias externas.
# =============================================================================
fix-libraries:
	@echo "--- Verificando integridad de librerias Kali ---"
	# 1. Arreglar typo en Kali App (struct faltante)
	-sed -i 's/typedef HINSTANCE__ Module;/typedef struct HINSTANCE__ Module;/g' ../libraries/kali/app.h
	# 2. Arreglar aritmetica de punteros void* en Kali Runtime
	-sed -i 's/\*dst++= \*src++;/*((char*\&)dst)++ = *((const char*\&)src)++;/g' ../libraries/kali/runtime.h
	# 3. Arreglar ambiguedad en Kali Window (String explicito)
	-sed -i 's/!comments ? text/!comments ? string("%s", text)/g' ../libraries/win/kali/window.h
	# 4. Eliminar redefinicion ilegal de DBG
	-sed -i 's/#define DBG (defined(_DEBUG))/\/\/#define DBG (defined(_DEBUG))/g' ../libraries/kali/dbgutils.h
	@echo "--- Librerias listas ---"

# =============================================================================
# COMPILACION Y ENLACE
# =============================================================================

# Linker: Generacion de la DLL final
$(TARGET): $(OBJS)
	@echo "--- Enlazando plugin: $(TARGET) ---"
	$(CXX) -shared -m32 -o $@ $(OBJS) $(LIBS) -Wl,--export-all-symbols -Wl,--kill-at -Wl,--defsym=main=VSTPluginMain
	@echo "--- C4 Analyzer compilado exitosamente ---"

# Compilacion del Wrapper VST (Incluye la logica del plugin)
vstsdk-wrapper.o: vstsdk-wrapper.cpp
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

# Compilacion del Motor DSP (Si sp.cpp existe en src/)
sp.o: sp.cpp
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

# Compilacion de Recursos (Iconos y Version)
main.res: main.rc
	windres main.rc -O coff -o main.res

# Limpieza
clean:
	rm -f *.o *.res *.dll

.PHONY: all fix-libraries clean
