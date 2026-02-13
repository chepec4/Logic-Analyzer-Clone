# =========================
# Configuración general
# =========================

CXX      := g++
CXXFLAGS := -O2 -std=gnu++03 -Wall -Wextra -fpermissive
DEFS     := -DWIN32 -D_WINDOWS
INCLUDES := \
	-I. \
	-I../libraries \
	-I../libraries/win \
	-I../libraries/vst \
	-I../libraries/vst/public.sdk/source/vst2.x \
	-I../libraries/vst/pluginterfaces/vst2.x \
	-I../libraries/sp

# =========================
# Archivos
# =========================

SRC := sp.cpp
OBJ := $(SRC:.cpp=.o)
BIN := sp_test.exe

# =========================
# Reglas
# =========================

all: $(BIN)

$(BIN): $(OBJ)
	$(CXX) $(OBJ) -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(DEFS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(OBJ) $(BIN)
