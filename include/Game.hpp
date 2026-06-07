#pragma once
#include <SFML/Graphics.hpp>
#include "Map.hpp"
#include "Player.hpp"

class Game {
private:
    sf::RenderWindow window;
    MapManager mapManager;
    Player player;
    
    const int SCREEN_WIDTH = 800;
    const int SCREEN_HEIGHT = 600;
    const float FOV = 3.14159f / 3.0f;
    const float MAX_DEPTH = 1500.0f;

    void processEvents();
    void update(float deltaTime);
    void renderRaycast();

public:
    Game();
    void run();
};