#ifndef MAP_HPP
#define MAP_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

struct Tile {
    int type;          // 0=vacío, 1=muro, 2=puerta, 3=secreto, 9=salida
    bool isOpen = false; // 🌟 NUEVO: Indica si la puerta está abierta
    float doorTimer = 0.0f; // 🌟 NUEVO: Cuenta los segundos que lleva abierta
};

// (Mantén el struct PushWall y Level exactamente como los tienes)
struct PushWall {
    bool isActive = false;
    int originalX, originalY;
    int targetX, targetY;
    float moveX = 0.0f;
    float moveY = 0.0f;
    float speed = 64.0f;
};

struct Level {
    std::string name;
    int rows;
    int cols;
    sf::Vector2f spawnPoint;
    std::vector<std::vector<Tile>> grid;
};

class MapManager {
private:
    std::vector<Level> levels;
    int currentLevelIndex;

public:
    bool showFullMap;
    PushWall activeSecret;

    MapManager();
    Level* getCurrentLevel();
    int getCurrentIndex() const;
    void nextLevel();
    
    bool isWall(int cellX, int cellY);
    bool isDoor(int cellX, int cellY);
    bool isExit(int cellX, int cellY);
    bool isSecret(int cellX, int cellY);
    
    void openDoor(int cellX, int cellY);
    void updateDoors(float deltaTime, sf::Vector2f playerPos); // 🌟 NUEVO: Cierra las puertas con el tiempo
    void triggerSecret(int cellX, int cellY, float playerAngle);
    void updateSecret(float deltaTime);
    
    void drawFullMap(sf::RenderWindow& window);
};

#endif