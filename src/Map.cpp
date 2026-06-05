#include "Map.hpp"

MapManager::MapManager() {
    currentLevelIndex = 0;
    showFullMap = false; // Empezar en modo 3D por defecto
    elevatorHeight = 0.0f;
    elevatorDirection = 1;
    elevatorSpeed = 0.6f;

    // NIVEL 1: Mapa inicial con una PUERTA (tipo 2) para validar que funcione
    Level lvl1;
    lvl1.name = "Hangar de Inicio";
    lvl1.rows = 6; lvl1.cols = 6;
    lvl1.spawnPoint = sf::Vector2f(96.0f, 96.0f);
    lvl1.grid.resize(6, std::vector<Tile>(6, {0, 0.0f, 1.0f, false}));
    
    for(int i = 0; i < 6; i++) {
        lvl1.grid[0][i].type = 1; lvl1.grid[5][i].type = 1;
        lvl1.grid[i][0].type = 1; lvl1.grid[i][6 - 1].type = 1;
    }
    // Colocamos una puerta intermedia y la salida detrás
    lvl1.grid[3][2] = {2, 0.0f, 1.0f, false}; // Puerta (Presiona Espacio frente a ella)
    lvl1.grid[4][4] = {9, 0.0f, 1.0f, false}; // Salida al Nivel 2
    levels.push_back(lvl1);

    // NIVEL 2: El sector tridimensional con escaleras y el elevador móvil
    Level lvl2;
    lvl2.name = "Sector de Elevadores";
    lvl2.rows = 7; lvl2.cols = 7;
    lvl2.spawnPoint = sf::Vector2f(96.0f, 96.0f);
    lvl2.grid.resize(7, std::vector<Tile>(7, {0, 0.0f, 1.0f, false}));
    
    for(int i = 0; i < 7; i++) {
        lvl2.grid[0][i].type = 1; lvl2.grid[6][i].type = 1;
        lvl2.grid[i][0].type = 1; lvl2.grid[i][6].type = 1;
    }

    // Escaleras estáticas
    lvl2.grid[2][2] = {0, 0.25f, 1.25f, false}; 
    lvl2.grid[2][3] = {0, 0.50f, 1.50f, false}; 
    lvl2.grid[2][4] = {0, 0.75f, 1.75f, false}; 

    // Elevador mecánico dinámico
    lvl2.grid[3][4] = {0, 0.0f, 1.75f, true}; 

    // Plataforma alta final y meta
    lvl2.grid[4][4] = {0, 1.0f, 2.0f, false}; 
    lvl2.grid[5][4] = {9, 0.0f, 1.0f, false}; 

    levels.push_back(lvl2);
}

Level* MapManager::getCurrentLevel() { return &levels[currentLevelIndex]; }
int MapManager::getCurrentIndex() const { return currentLevelIndex; }

void MapManager::nextLevel() { 
    currentLevelIndex = (currentLevelIndex + 1) % levels.size(); 
}

bool MapManager::isWall(int cellX, int cellY) {
    Level* cur = getCurrentLevel();
    if (cellX < 0 || cellX >= cur->cols || cellY < 0 || cellY >= cur->rows) return true;
    return cur->grid[cellY][cellX].type == 1;
}

bool MapManager::isExit(int cellX, int cellY) {
    Level* cur = getCurrentLevel();
    if (cellX < 0 || cellX >= cur->cols || cellY < 0 || cellY >= cur->rows) return false;
    return cur->grid[cellY][cellX].type == 9;
}

bool MapManager::isDoor(int cellX, int cellY) {
    Level* cur = getCurrentLevel();
    if (cellX < 0 || cellX >= cur->cols || cellY < 0 || cellY >= cur->rows) return false;
    return cur->grid[cellY][cellX].type == 2;
}

void MapManager::openDoor(int cellX, int cellY) {
    Level* cur = getCurrentLevel();
    if (cellX >= 0 && cellX < cur->cols && cellY >= 0 && cellY < cur->rows) {
        if (cur->grid[cellY][cellX].type == 2) {
            cur->grid[cellY][cellX].type = 0; // Se abre removiendo el obstáculo
        }
    }
}

void MapManager::updateElevator(float deltaTime) {
    if (currentLevelIndex != 1) return;
    Level* cur = getCurrentLevel();

    elevatorHeight += elevatorSpeed * elevatorDirection * deltaTime;

    if (elevatorHeight >= 1.0f) {
        elevatorHeight = 1.0f;
        elevatorDirection = -1;
    } else if (elevatorHeight <= 0.0f) {
        elevatorHeight = 0.0f;
        elevatorDirection = 1;
    }

    for (int r = 0; r < cur->rows; r++) {
        for (int c = 0; c < cur->cols; c++) {
            if (cur->grid[r][c].isElevator) {
                cur->grid[r][c].floorHeight = elevatorHeight;
            }
        }
    }
}

void MapManager::drawFullMap(sf::RenderWindow& window) {
    Level* cur = getCurrentLevel();
    int tileSize = 32;
    float startX = (window.getSize().x - (cur->cols * tileSize)) / 2.0f;
    float startY = (window.getSize().y - (cur->rows * tileSize)) / 2.0f;
    sf::RectangleShape tile(sf::Vector2f(tileSize - 2, tileSize - 2));

    for (int r = 0; r < cur->rows; r++) {
        for (int c = 0; c < cur->cols; c++) {
            Tile cell = cur->grid[r][c];
            if (cell.type == 1) tile.setFillColor(sf::Color(100, 100, 100));
            else if (cell.type == 2) tile.setFillColor(sf::Color(150, 50, 150)); // Puertas violetas
            else if (cell.isElevator) tile.setFillColor(sf::Color(230, 130, 40)); 
            else if (cell.floorHeight > 0.0f) tile.setFillColor(sf::Color(40, 110, 190)); 
            else tile.setFillColor(sf::Color(35, 35, 35));

            tile.setPosition(startX + c * tileSize, startY + r * tileSize);
            window.draw(tile);
        }
    }
}