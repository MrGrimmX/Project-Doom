#ifndef MAP_HPP
#define MAP_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

struct Tile {
    int type;            // 0 = vacío, 1 = muro, 2 = puerta, 9 = salida
    float floorHeight;   // Altura del suelo (0.0f normal, >0.0f plataformas)
    float ceilingHeight; // Altura del techo
    bool isElevator;     // ¿Es una plataforma móvil?
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
    float elevatorHeight;   
    int elevatorDirection; 
    float elevatorSpeed;

    MapManager();
    
    Level* getCurrentLevel();
    int getCurrentIndex() const;
    void nextLevel();
    
    bool isWall(int cellX, int cellY);
    bool isExit(int cellX, int cellY);
    bool isDoor(int cellX, int cellY);
    void openDoor(int cellX, int cellY);
    
    void updateElevator(float deltaTime);
    void drawFullMap(sf::RenderWindow& window);
};

#endif