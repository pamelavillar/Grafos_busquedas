# Nombre del ejecutable
TARGET = grafo_app

# Compilador
CXX = clang++

# Flags de compilación
CXXFLAGS = -I/opt/homebrew/include -DGL_SILENCE_DEPRECATION

# Librerías
LDFLAGS = -L/opt/homebrew/lib -lglfw -framework OpenGL

# Archivos fuente
SRC = main.cpp

# Regla principal
all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(SRC) $(CXXFLAGS) $(LDFLAGS) -o $(TARGET)

# Limpiar archivos generados
clean:
	rm -f $(TARGET)
