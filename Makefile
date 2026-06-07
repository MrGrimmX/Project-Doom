# Variables globales
CXX      = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -Iinclude
LDFLAGS  = -lsfml-graphics -lsfml-window -lsfml-system

# Carpetas del proyecto
SRC_DIR = src
INC_DIR = include
BIN_DIR = bin
OBJ_DIR = obj

# Encontrar archivos fuentes
SRCS = $(wildcard $(SRC_DIR)/*.cpp)
# Colocar los archivos .o dentro de su propia carpeta dedicada 'obj/'
OBJS = $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

TARGET = $(BIN_DIR)/raycaster

all: $(TARGET)

# Enlace del ejecutable final
$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(OBJS) -o $(TARGET) $(LDFLAGS)
	@echo "✅ ¡Proyecto compilado con éxito en $(TARGET)!"

# Regla para compilar los .cpp a .o guardándolos en la carpeta obj/
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(TARGET)
	@echo "🧹 Carpeta obj/ y ejecutable eliminados."

.PHONY: all clean