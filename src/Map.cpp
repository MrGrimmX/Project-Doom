#include "Map.hpp"
#include "Level.hpp"
#include <iostream>
#include <cmath>

MapManager::MapManager() {
    currentLevelIndex = 0;
    showFullMap = false;

    // Cargar los niveles planos estilo Wolfenstein
    levels = getBuiltInLevels();
}

Level* MapManager::getCurrentLevel() { 
    if (levels.empty()) {
        static Level dummy = {"Dummy", 1, 1, {96.f, 96.f}, {{{0}}}};
        return &dummy;
    }
    return &levels[currentLevelIndex]; 
}

int MapManager::getCurrentIndex() const { return currentLevelIndex; }

void MapManager::nextLevel() { 
    if (!levels.empty()) {
        currentLevelIndex = (currentLevelIndex + 1) % levels.size(); 
    }
}

bool MapManager::isWall(int cellX, int cellY) {
    Level* cur = getCurrentLevel();
    if (cellX < 0 || cellX >= cur->cols || cellY < 0 || cellY >= cur->rows) return true;
    
    // Si la pared secreta se está desplazando, calculamos su colisión dinámica
    if (activeSecret.isActive) {
        float currentXpx = activeSecret.originalX * 64 + activeSecret.moveX;
        float currentYpx = activeSecret.originalY * 64 + activeSecret.moveY;

        // Evaluar extremos del bloque (el frente y la parte de atrás)
        int occupiedCellX1 = (int)(currentXpx) / 64;
        int occupiedCellY1 = (int)(currentYpx) / 64;
        int occupiedCellX2 = (int)(currentXpx + 63.0f) / 64;
        int occupiedCellY2 = (int)(currentYpx + 63.0f) / 64;

        if ((cellX == occupiedCellX1 && cellY == occupiedCellY1) ||
            (cellX == occupiedCellX2 && cellY == occupiedCellY2)) {
            return true;
        }

        if (cellX == activeSecret.targetX && cellY == activeSecret.targetY) {
            return true;
        }
    }

    Tile cell = cur->grid[cellY][cellX];
    if (cell.type == 1) return true;
    if (cell.type == 3) return true; // Sólido mientras no se active
    if (cell.type == 2 && !cell.isOpen) return true;

    return false;
}

bool MapManager::isDoor(int cellX, int cellY) {
    Level* cur = getCurrentLevel();
    if (cellX < 0 || cellX >= cur->cols || cellY < 0 || cellY >= cur->rows) return false;
    return cur->grid[cellY][cellX].type == 2 && !cur->grid[cellY][cellX].isOpen;
}

bool MapManager::isExit(int cellX, int cellY) {
    Level* cur = getCurrentLevel();
    if (cellX < 0 || cellX >= cur->cols || cellY < 0 || cellY >= cur->rows) return false;
    return cur->grid[cellY][cellX].type == 9;
}

void MapManager::openDoor(int cellX, int cellY) {
    Level* cur = getCurrentLevel();
    if (cellX >= 0 && cellX < cur->cols && cellY >= 0 && cellY < cur->rows) {
        if (cur->grid[cellY][cellX].type == 2) {
            cur->grid[cellY][cellX].isOpen = true;
            cur->grid[cellY][cellX].doorTimer = 0.0f; // Reiniciar el contador a 0
        }
    }
}

void MapManager::drawFullMap(sf::RenderWindow& window) {
    if (levels.empty()) return;
    Level* cur = getCurrentLevel();
    int tileSize = 32;
    float startX = (window.getSize().x - (cur->cols * tileSize)) / 2.0f;
    float startY = (window.getSize().y - (cur->rows * tileSize)) / 2.0f;
    sf::RectangleShape tile(sf::Vector2f(tileSize - 2, tileSize - 2));

    for (int r = 0; r < cur->rows; r++) {
    for (int c = 0; c < cur->cols; c++) {
        Tile cell = cur->grid[r][c];
        
        // 🌟 Si es muro normal (1) O muro secreto (3), pintarlo del mismo gris
        if (cell.type == 1 || cell.type == 3) {
            tile.setFillColor(sf::Color(100, 100, 100)); // Gris de muro
        }     
        else if (cell.type == 2) {
            tile.setFillColor(sf::Color(150, 50, 150)); // Puerta Violeta
        } 
        else if (cell.type == 9) {
            tile.setFillColor(sf::Color(35, 180, 70));   // Salida Verde
        }   
        else {
            tile.setFillColor(sf::Color(35, 35, 35));    // Suelo libre
        }

        tile.setPosition(startX + c * tileSize, startY + r * tileSize);
        window.draw(tile);
    }
}
}

bool MapManager::isSecret(int cellX, int cellY) {
    Level* cur = getCurrentLevel();
    if (cellX < 0 || cellX >= cur->cols || cellY < 0 || cellY >= cur->rows) return false;
    return cur->grid[cellY][cellX].type == 3;
}

void MapManager::triggerSecret(int cellX, int cellY, float playerAngle) {
    // Si ya hay un muro secreto moviéndose, ignorar
    if (activeSecret.isActive) return;

    Level* cur = getCurrentLevel();

    // Determinar la dirección del empuje basándonos en el ángulo del jugador
    int dirX = 0;
    int dirY = 0;

    // Normalizar ángulo entre 0 y 2*PI
    float normAngle = playerAngle;
    while (normAngle < 0) normAngle += 2 * 3.14159265f;
    while (normAngle >= 2 * 3.14159265f) normAngle -= 2 * 3.14159265f;

    // Clasificar en las 4 direcciones principales (Norte, Sur, Este, Oeste)
    if (normAngle >= 7.0f * 3.14159265f / 4.0f || normAngle < 3.14159265f / 4.0f) dirX = 1;  // Este (Derecha)
    else if (normAngle >= 1.0f * 3.14159265f / 4.0f && normAngle < 3.0f * 3.14159265f / 4.0f) dirY = 1;  // Sur (Abajo)
    else if (normAngle >= 3.0f * 3.14159265f / 4.0f && normAngle < 5.0f * 3.14159265f / 4.0f) dirX = -1; // Oeste (Izquierda)
    else dirY = -1; // Norte (Arriba)

    int tX = cellX + dirX;
    int tY = cellY + dirY;

    // Verificar si la celda de destino está dentro del mapa y está vacía
    if (tX >= 0 && tX < cur->cols && tY >= 0 && tY < cur->rows && cur->grid[tY][tX].type == 0) {
        activeSecret.isActive = true;
        activeSecret.originalX = cellX;
        activeSecret.originalY = cellY;
        activeSecret.targetX = tX;
        activeSecret.targetY = tY;
        activeSecret.moveX = 0.0f;
        activeSecret.moveY = 0.0f;

        // Vaciamos temporalmente la celda de origen para que el jugador pueda entrar al pasaje
        cur->grid[cellY][cellX].type = 0;
    }
}

void MapManager::updateSecret(float deltaTime) {
    if (!activeSecret.isActive) return;

    // Calcular cuánto debe avanzar en este frame
    int dirX = activeSecret.targetX - activeSecret.originalX;
    int dirY = activeSecret.targetY - activeSecret.originalY;

    activeSecret.moveX += dirX * activeSecret.speed * deltaTime;
    activeSecret.moveY += dirY * activeSecret.speed * deltaTime;

    // Comprobar si completó el recorrido de 1 celda (64 unidades físicas)
    float distanceMoved = std::sqrt(activeSecret.moveX * activeSecret.moveX + activeSecret.moveY * activeSecret.moveY);
    if (distanceMoved >= 64.0f) {
        Level* cur = getCurrentLevel();
        // Consolidar permanentemente el muro en su celda de destino
        cur->grid[activeSecret.targetY][activeSecret.targetX].type = 1; // Se vuelve muro normal fijo

        // Desactivar el estado de movimiento
        activeSecret.isActive = false;
    }
}

void MapManager::updateDoors(float deltaTime, sf::Vector2f playerPos) {
    Level* cur = getCurrentLevel();
    
    // Calcular en qué celda está parado el jugador actualmente
    int playerCellX = (int)(playerPos.x) / 64;
    int playerCellY = (int)(playerPos.y) / 64;

    for (int r = 0; r < cur->rows; r++) {
        for (int c = 0; c < cur->cols; c++) {
            if (cur->grid[r][c].type == 2 && cur->grid[r][c].isOpen) {
                // Sumar el tiempo transcurrido
                cur->grid[r][c].doorTimer += deltaTime;

                // Si ya pasaron 5 segundos...
                if (cur->grid[r][c].doorTimer >= 5.0f) {
                    
                    // SEGURIDAD: No cerrar la puerta si el jugador está parado justo en medio de ella
                    if (c == playerCellX && r == playerCellY) {
                        cur->grid[r][c].doorTimer = 3.0f; // Darle 2 segundos más de prórroga
                        continue;
                    }

                    // Cerrar la puerta
                    cur->grid[r][c].isOpen = false;
                    cur->grid[r][c].doorTimer = 0.0f;
                }
            }
        }
    }
}