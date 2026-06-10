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
OBJS = $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
TARGET = $(BIN_DIR)/raycaster

all: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(OBJS) -o $(TARGET) $(LDFLAGS)
	@echo "✅ ¡Proyecto compilado con éxito en $(TARGET)!"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(TARGET)
	@echo "🧹 Limpieza completada"

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run